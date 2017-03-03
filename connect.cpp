#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <assert.h>
#include "rasocket.h"
#include "connect.h"
#include "usage.h"

static SOCKET sd;
#define BUFFER_SIZE (ssize_t)256

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
        perror(NULL);
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
        perror(NULL);
        return false;
    }

    sprintf(buffer,"test message from socker's sconnect function");
    ssize_t writeSoFar = 0;
    void * ptr = buffer;

    while (writeSoFar < BUFFER_SIZE)
    {
        ssize_t writeC = 0;

        assert( writeSoFar < BUFFER_SIZE);
        if((writeC = write(sd,ptr,(size_t)(BUFFER_SIZE - writeSoFar))) == -1)
        {
            return false;
        }
        writeSoFar = writeSoFar + writeC;
        ptr = (char *)ptr + writeC;

    }
    return true;
}

