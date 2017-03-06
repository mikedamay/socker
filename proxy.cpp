#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32)
#include <sys/un.h>
#endif
#include "rasocket.h"
#include "proxy.h"
#include "usage.h"
#include "utils.h"


bool sproxy_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"proxy"
            ,(char *)"socker <action> <proxyhostname> <proxyportnumber> <proxiedhostname> <proxiedportnumber>\n\te.g. socker proxy localhost 11900 remotehost 80\n"
            , usageStr, usageLen);
}

bool populateSockAddr(char * host, unsigned short port, sockaddr_in * psin)
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
/*
    printf("socker:proxy() about to bind on proxyPort\n");
    if (bind(sdProxyServer, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == BIND_FAILED)
    {
        printError(NULL);
        return false;
    }
*/

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

            SOCKET sd2;
            printf("socker:proxy() about to call accept()\n");
            if ((sd2 = accept(sdProxyServer, NULL, NULL)) == SOCKET_FAILED)
            {
                printError("accept failed:");
                return false;
            }
            char buffer[1024];
            printf("socker:proxy() about to call read()\n");
            ssize_t numBytes;
            do
            {
                numBytes = recv(sd2, buffer, sizeof(buffer), 0);
                if (numBytes == -1)
                {
                    printError("socker:proxy() read failed:");
                    break;
                }
                else if (numBytes > 0)
                {
                    printf("socker:proxy() about to print buffer\n");
                    printf(buffer);
                    printf("\n");
                    send(sd2, buffer, sizeof buffer, 0);
                }
                else
                {
                    printf("socker:proxy() read of zero bytes performed\n");
                    printError("socker:proxy() zero bytes read error:");
                    break;
                }
            }
            while (true);   // while reading a message
            close(sd2);
        }   // while listening
    }
}