//
// Created by mike on 3/11/17.
//
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include "rasocket.h"
#include "multi_proxy.h"
#include "usage.h"
#include "oneTimeBuffer.h"
#include "rasocket.h"
#include "utils.h"
#include "sessionHandler.h"

bool smulti_proxy_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"multi_proxy"
    ,(char *)"socker multi_proxy <proxyhostname> <proxyportnumber> <proxiedhostname> <proxiedportnumber>\n\te.g. socker multi_proxy localhost 12900 remotehost 11900\n\tforwards any messages from client to remotehost and vice versa\n"
    , usageStr, usageLen);
}

bool smulti_proxy(int argc, char ** argv)
{
    char proxyHost[ADDRESS_SIZE];
    unsigned short proxyPort;
    if (!getHostAndPort(argc, argv, ARG_REMOTE_HOST, ARG_PORT, proxyHost, ADDRESS_SIZE, &proxyPort))
    {
        exit(255);
    }
    char proxiedHost[ADDRESS_SIZE];
    unsigned short proxiedPort;
    if (!getHostAndPort(argc, argv, ARG_PORT + 1, ARG_PORT + 2, proxiedHost, ADDRESS_SIZE, &proxiedPort))
    {
        exit(255);
    }

    SOCKET sdProxyServer;
    if ((sdProxyServer = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
    {
        printError(NULL);
        return false;
    }
    printf("socker:multi_proxy() socket created\n");
    struct sockaddr_in sin;
    if (!populateSockAddr(proxyHost, proxyPort, &sin))
    {
        return false;
    }
    printf("socker:multi_proxy() about to bind on proxyPort\n");
    if (bind(sdProxyServer, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == BIND_FAILED)
    {
        printError(NULL);
        return false;
    }
    // listen for connections and echo messages back to the peer
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    if (shh == SESSION_HANDLER_HANDLE_ERROR)
    {
        perror("failed to create session handler");
        return false;
    }
    printf("socket:proxy()  about to call listen()\n");
    SOCKET sdProxied;
    if (listen(sdProxyServer, 10) == SOCKET_FAILED)
    {
        printError("listen failed:");
        return false;
    }
    else
    {
        SOCKET sdClient;
        char buffer[1024];
        ssize_t numBytes;
        ONE_TIME_BUFFER_HANDLE fromClientBuffer;
        ONE_TIME_BUFFER_HANDLE  toClientBuffer;
        fd_set rfds;
        fd_set wfds;
        fd_set exceptfds;
        do
        {
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_ZERO(&exceptfds);
            FD_SET(sdProxyServer, &rfds);
            FD_SET(sdProxyServer, &exceptfds);
            SESSION_ITER si = getSessionIter(shh);
            if (si == SESSION_ITER_ERROR)
            {
                printError("failed to create session iter");
                break;
            }
            while (getNextSession(si, &sdClient, &sdProxied, &fromClientBuffer, &toClientBuffer))
            {
                FD_SET(sdProxied, &rfds);
                FD_SET(sdProxied, &exceptfds);
                FD_SET(sdClient, &rfds);
                FD_SET(sdClient, &exceptfds);
                if (availableBytesInOTB(fromClientBuffer) > 0)
                {
                    FD_SET(sdProxied, &wfds);
                }
                if (availableBytesInOTB(toClientBuffer) > 0)
                {
                    FD_SET(sdClient, &wfds);
                }
            }
            si = SESSION_ITER_ERROR;
            int fdCap = max(sdProxyServer, getMaxSD(shh)) + 1;
            select(fdCap, &rfds, &wfds, &exceptfds, NULL);
            if (FD_ISSET(sdProxyServer, &exceptfds))
            {
                printf("socker:multi_proxy() exception in select - proxy server socket\n");
            }
            if (FD_ISSET(sdProxyServer, &rfds))
            {       // new connection from a client
                printf("socker:multi_proxy() about to call accept()\n");
                if ((sdClient = accept(sdProxyServer, NULL, NULL)) == SOCKET_FAILED)
                {
                    printError("accept failed:");
                    break;
                }
                printf("socker:multi_proxy() accepted client %d\n", sdClient);
                struct sockaddr_in sinProxied;
                if (!populateSockAddr(proxiedHost, proxiedPort, &sinProxied))
                {
                    return false;
                }
                printf("socker:multi_proxy() about to create proxied socket\n");
                if ((sdProxied = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
                {
                    char errbuff[SCRATCH_BUFFER_SIZE];
                    sprintf(errbuff,"socker:multi_proxy() failed to connect to proxied socket %s %d\n", proxiedHost, proxiedPort);
                    printError(errbuff);
                    break;
                }
                printf("socker:multi_proxy() about to connect to proxied socket %d\n", sdProxied);
                if((connect(sdProxied,(struct sockaddr *)&sinProxied,sizeof(sinProxied))) == CONNECT_FAILED)
                {
                    printError(NULL);
                    break;
                }
                char scratch[1024];
                sprintf(scratch, "From Client %d", sdClient);
                fromClientBuffer = createOneTimeBuffer(scratch);
                sprintf(scratch, "To Client %d", sdProxied);
                toClientBuffer = createOneTimeBuffer(scratch);
                if (fromClientBuffer == NULL || toClientBuffer == NULL)
                {
                    printf("%s", "out of memory adding client");
                    return false;
                }
                if (!addSession(shh, sdClient, sdProxied, fromClientBuffer, toClientBuffer))
                {
                    printf("%s", "out of memory creating session");
                    return false;
                }
            }
            si = getSessionIter(shh);
            if (si == SESSION_ITER_ERROR)
            {
                perror("failed to create session iter");
                break;
            }
            while (getNextSession(si, &sdClient, &sdProxied, &fromClientBuffer, &toClientBuffer))
            {
                if (FD_ISSET(sdProxied, &exceptfds) || FD_ISSET(sdClient, &exceptfds))
                {
                    printf("socker:multi_proxy() exception in select\n");
                }
                if (FD_ISSET(sdClient, &rfds))
                {
                    memset(buffer, '\0', sizeof(buffer));
                    numBytes = recv(sdClient, buffer, sizeof(buffer), 0);
                    if (numBytes == -1)
                    {
                        printError("socker:multi_proxy() read failed:");
                        break;
                    }
                    else if (numBytes > 0)
                    {
                        printf("socker:multi_proxy() about to print buffer from client %d\n", sdClient);
                        //printf("%s", buffer);
                        //printf("\n");
                        writeOTB(fromClientBuffer, buffer, numBytes);
                    }
                    else
                    {
                        printf("socker:multi_proxy() read of zero bytes performed from %d\n", sdClient);
                        printError("socker:multi_proxy() zero bytes read error from client");
                        close(sdClient);
                        close(sdProxied);
                        destroyOneTimeBuffer(fromClientBuffer);
                        destroyOneTimeBuffer(toClientBuffer);
                        removeSession(shh, sdClient);
                        break;
                    }
                }
                if (FD_ISSET(sdProxied, &rfds))
                {
                    memset(buffer, '\0', sizeof(buffer));
                    numBytes = recv(sdProxied, buffer, sizeof(buffer), 0);
                    if (numBytes == -1)
                    {
                        printError("socker:multi_proxy() sdProxied read failed:");
                        break;
                    }
                    else if (numBytes > 0)
                    {
                        printf("socker:multi_proxy() about to print buffer from proxied %d\n", sdProxied);
                        //printf("%s", buffer);
                        //printf("\n");
                        writeOTB(toClientBuffer, buffer, numBytes);
                    }
                    else
                    {
                        printf("socker:multi_proxy() proxied read of zero bytes performed from %d\n", sdProxied);
                        printError("socker:multi_proxy() zero bytes read error from proxied");
                        close(sdClient);
                        close(sdProxied);
                        destroyOneTimeBuffer(fromClientBuffer);
                        destroyOneTimeBuffer(toClientBuffer);
                        removeSession(shh, sdClient);
                        break;
                    }
                }
                if (FD_ISSET(sdProxied, &wfds))
                {
                    ssize_t numBytesToWrite;
                    char * writeBuffer;
                    if (getAndLockOTBForRead(fromClientBuffer, &writeBuffer, &numBytesToWrite))
                    {
                        printf("socker:multi_proxy() about to send to proxied %d -> %d\n", sdClient, sdProxied);
                        ssize_t numBytesWritten = send(sdProxied, writeBuffer, (size_t)numBytesToWrite, 0);
                        unlockOTB(fromClientBuffer, numBytesWritten);
                    }
                }
                if (FD_ISSET(sdClient, &wfds))
                {
                    ssize_t numBytesToWrite;
                    char * writeBuffer;
                    if (getAndLockOTBForRead(toClientBuffer, &writeBuffer, &numBytesToWrite))
                    {
                        printf("socker:multi_proxy() about to send to client %d -> %d\n", sdProxied, sdClient);
                        ssize_t numBytesWritten = send(sdClient, writeBuffer, (size_t)numBytesToWrite, 0);
                        unlockOTB(toClientBuffer, numBytesWritten);
                    }
                }
            }       // getNextSession()
            si = SESSION_ITER_ERROR;
        } while (true);   // while reading a message
    }
}