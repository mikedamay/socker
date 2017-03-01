#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <assert.h>
#include "rasocket.h"

#ifdef unix
#define SOCKET int
#include <unistd.h>
#endif

static SOCKET sd;
#define BUFFER_SIZE (ssize_t)256
#define ADDRESS_SIZE 108
#define DEFAULT_PORT 80

int connect(int argc, char **argv)
{
    struct hostent * server;
    struct sockaddr_in them;
    int port = 0;
    char machine[ADDRESS_SIZE];
    char buffer[BUFFER_SIZE];

    strcpy(machine,"");
    if(argc > 1)
    {
        if(!(strcmp(argv[1],"-usage")))
        {
            printf("usage : %s <machinename> <portno>\n\te.g. %s localhost 80\n",argv[0], argv[0]);
            exit(1);
        }
        else if (argc > 1) {
            strcpy(machine,argv[1]);
        }
    }

    if(argc == 3)
        port = atoi(argv[2]);

    if((sd = socket(AF_INET,SOCK_STREAM,0)) == SOCKET_FAILED)
    {
        perror(NULL);
        return false;
    }

    if(strcmp(machine,"")==0)
        gethostname(machine,ADDRESS_SIZE);

    if(port == 0)
        port = DEFAULT_PORT;

    printf("attempting connection to machine : %s port : %d\n",machine,port);
    server = gethostbyname(machine);
    if(server == NULL) return false;
    memset(&them,0,sizeof(them));
    memcpy((char *)&them.sin_addr,server->h_addr,(unsigned short)server->h_length);
    them.sin_family = AF_INET;
    them.sin_port = htons((unsigned short)port);

    if((connect(sd,(struct sockaddr *)&them,sizeof(them))) == CONNECT_FAILED)
    {
        perror(NULL);
        return false;
    }

    sprintf(buffer,"test message from socker's connect function");
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

