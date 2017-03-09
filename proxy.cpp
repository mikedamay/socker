#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32)
#include <sys/un.h>
#include <netinet/in.h>
#endif
#include "rasocket.h"
#include "proxy.h"
#include "usage.h"
#include "utils.h"
#include "oneTimeBuffer.h"

#define max(a,b) (a>b?a:b)

bool sproxy_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"proxy"
            ,(char *)"socker proxy <proxyhostname> <proxyportnumber> <proxiedhostname> <proxiedportnumber>\n\te.g. socker proxy localhost 12900 remotehost 11900\n\tforwards any messages from client to remotehost and vice versa\n"
            , usageStr, usageLen);
}

bool populateSockAddr(char * host, unsigned short port, struct sockaddr_in * psin)
{
    struct hostent * pserver = gethostbyname(host);
    if (pserver == NULL)
    {
        printError(NULL);
        return false;
    }
    psin->sin_family = AF_INET;
    psin->sin_port = htons(port);
    memcpy(&psin->sin_addr.s_addr, pserver->h_addr_list[0], (unsigned short)pserver->h_length);
    memset(&psin->sin_zero, 0, sizeof psin->sin_zero);
    return true;
}

bool sproxy(int argc, char ** argv)
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
    printf("socker:proxy() socket created\n");
    struct sockaddr_in sin;
    if (!populateSockAddr(proxyHost, proxyPort, &sin))
    {
        return false;
    }
    printf("socker:proxy() about to bind on proxyPort\n");
    if (bind(sdProxyServer, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == BIND_FAILED)
    {
        printError(NULL);
        return false;
    }
    SOCKET sdProxied;
    struct sockaddr_in sinProxied;
    if (!populateSockAddr(proxiedHost, proxiedPort, &sinProxied))
    {
        return false;
    }
    printf("about to create proxied socket\n");
    if ((sdProxied = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
    {
        char errbuff[SCRATCH_BUFFER_SIZE];
        sprintf(errbuff,"socker:proxy() failed to connect to proxied socket %s %d\n", proxiedHost, proxiedPort);
        printError(errbuff);
        return false;
    }
    printf("about to connect to proxied socket\n");
    if((connect(sdProxied,(struct sockaddr *)&sinProxied,sizeof(sinProxied))) == CONNECT_FAILED)
    {
        printError(NULL);
        return false;
    }

    // listen for connections and echo messages back to the peer
    while (true)
    {
        printf("socket:proxy()  about to call listen()\n");
        if (listen(sdProxyServer, 10) == SOCKET_FAILED)
        {
            printError("listen failed:");
            return false;
        }
        else
        {
            SOCKET sdClient;
            printf("socker:proxy() about to call accept()\n");
            if ((sdClient = accept(sdProxyServer, NULL, NULL)) == SOCKET_FAILED)
            {
                printError("accept failed:");
                return false;
            }
            char buffer[1024];
            printf("socker:proxy() about to call read()\n");
            ONE_TIME_BUFFER_HANDLE fromClientBuffer = createOneTimeBuffer((char *)"From Client");
            ONE_TIME_BUFFER_HANDLE  toClientBuffer = createOneTimeBuffer((char * )"To Client");
            ssize_t numBytes;
            do
            {
                fd_set rfds;
                fd_set wfds;
                fd_set exceptfds;
                FD_ZERO(&rfds);
                FD_ZERO(&wfds);
                FD_ZERO(&exceptfds);
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
                int fdCap = max(sdClient, sdProxied) + 1;
                select(fdCap, &rfds, &wfds, &exceptfds, NULL);
                if (FD_ISSET(sdProxied, &exceptfds) || FD_ISSET(sdClient, &exceptfds))
                {
                    printf("socker:proxy() exception in select\n");
                }
                if (FD_ISSET(sdClient, &rfds))
                {
                    memset(buffer, '\0', sizeof(buffer));
                    numBytes = recv(sdClient, buffer, sizeof(buffer), 0);
                    if (numBytes == -1)
                    {
                        printError("socker:proxy() read failed:");
                        break;
                    }
                    else if (numBytes > 0)
                    {
                        printf("socker:proxy() about to print buffer from client\n");
                        printf(buffer);
                        printf("\n");
                        writeOTB(fromClientBuffer, buffer, numBytes);
                    }
                    else
                    {
                        printf("socker:proxy() read of zero bytes performed\n");
                        printError("socker:proxy() zero bytes read error from client");
                        break;
                    }
                }
                if (FD_ISSET(sdProxied, &rfds))
                {
                    memset(buffer, '\0', sizeof(buffer));
                    numBytes = recv(sdProxied, buffer, sizeof(buffer), 0);
                    if (numBytes == -1)
                    {
                        printError("socker:proxy() sdProxied read failed:");
                        break;
                    }
                    else if (numBytes > 0)
                    {
                        printf("socker:proxy() about to print buffer from proxied\n");
                        printf(buffer);
                        printf("\n");
                        writeOTB(toClientBuffer, buffer, numBytes);
                    }
                    else
                    {
                        printf("socker:proxy() proxied read of zero bytes performed\n");
                        printError("socker:proxy() zero bytes read error from proxied");
                        break;
                    }
                }
                if (FD_ISSET(sdProxied, &wfds))
                {
                    ssize_t numBytesToWrite;
                    char * writeBuffer;
                    if (getAndLockOTBForRead(fromClientBuffer, &writeBuffer, &numBytesToWrite))
                    {
                        printf("socker:proxy() about to send to proxied\n");
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
                        printf("socker:proxy() about to send to client");
                        ssize_t numBytesWritten = send(sdClient, writeBuffer, (size_t)numBytesToWrite, 0);
                        unlockOTB(toClientBuffer, numBytesWritten);
                    }
                }
            } while (true);   // while reading a message

            close(sdClient);
        }   // while listening
    }
}