#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32) 
#include <sys/un.h>
#endif
#include <assert.h>
#include "rasocket.h"
#include "connect.h"
#include "usage.h"

static SOCKET sd;
#define BUFFER_SIZE (ssize_t)256

static void ssleep(int seconds)
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);

}

bool sconnect_usage(int argc, char **argv, char * usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"connect"
      ,(char *)"socker <action> <machinename> <portno>\n\te.g. socker connect localhost 80\n"
      , usageStr, usageLen);
}

bool sconnect(char * remoteServer, unsigned short remotePort)
{
    struct hostent * server;
    struct sockaddr_in them;
    char buffer[BUFFER_SIZE];

    if((sd = socket(AF_INET,SOCK_STREAM,0)) == SOCKET_FAILED)
    {
		printError(NULL);
        return false;
    }

    printf("attempting connection to machine : %s port : %d\n",remoteServer,remotePort);
    server = gethostbyname(remoteServer);
    if(server == NULL) return false;
    memset(&them,0,sizeof(them));
    memcpy((char *)&them.sin_addr,server->h_addr_list[0],(unsigned short)server->h_length);
    them.sin_family = AF_INET;
    them.sin_port = htons(remotePort);

    if((connect(sd,(struct sockaddr *)&them,sizeof(them))) == CONNECT_FAILED)
    {
		printError(NULL);
        return false;
    }

    sprintf(buffer,"test message from socker's sconnect function");
    ssize_t writeSoFar = 0;
    void * ptr = buffer;

    while (writeSoFar < BUFFER_SIZE)
    {
        ssize_t writeC = 0;

        assert( writeSoFar < BUFFER_SIZE);
        if((writeC = send(sd,(const char *)ptr,(size_t)(BUFFER_SIZE - writeSoFar), 0)) == -1)
        {
			printError("write failed:");
            return false;
        }
        writeSoFar = writeSoFar + writeC;
        ptr = (char *)ptr + writeC;

    }
    char buffer2[1024];
    printf("socker:bind() about to call read()\n");
    ssize_t numBytes;
    do
    {
        numBytes = recv(sd, buffer2, sizeof(buffer2), 0);
        if (numBytes == -1)
        {
			printError("socker:connect() read failed:");
            break;
        }
        else if (numBytes > 0)
        {
            printf("socker:connect() about to print buffer\n");
            printf(buffer2);
            printf("\n");
        }
        else
        {
            printf("socker:connect() read of zero bytes performed\n");
        }
        ioctl_byte_count_t availableBytes;
        if (ioctl(sd, FIONREAD, &availableBytes) != 0)
        {
			printError("socker:connect() ioctl failed:");
            break;
        }
        else
        {
            if (availableBytes == 0)
            {
                break;
            }
        }
    }
    while (true);
    //int res = shutdown(sd, SHUT_RDWR);
    close(sd);
    return true;
}

