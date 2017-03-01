#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifndef PSOCKET_DEF
#define PSOCKET_DEF

#ifdef unix
#define RASOCK int
#define INVALID_SOCKET 0
#define RASOCKERR -1
#define RASOCKIOCTL ioctl
#define RASOCKCLOSE close
#include <unistd.h>
#else
#define RASOCK SOCKET
#define RASOCKERR SOCKET_ERROR
#define RASOCKIOCTL ioctlsocket
#define RASOCKCLOSE closesocket
#endif

#endif

#include "rasocket.h"
#include <sys/un.h>
#include <assert.h>
#include <errno.h>
#include <error.h>

static int s_replyMode;
static RASOCK sd = INVALID_SOCKET;
static int fam;


#define REALTIME_BUFFER_SIZE (ssize_t)256

static struct sockaddr_in m_them;
static struct sockaddr_un un;
#define FALSE 0


int connect(int argc, char **argv)
{
    struct hostent * server;
    struct sockaddr_in them;
    int port = 0;
    char machine[80];

    char buffer[256];

    strcpy(machine,"nobody");
    if(argc > 1)
        strcpy(machine,argv[1]);
    if(argc > 1)
    {
        if(!(strcmp(argv[1],"-usage")))
        {
            printf("usage : %s machinename portno\n",argv[0]);
            exit(1);
        }
    }

    if(argc == 3)
        port = atoi(argv[2]);

    fam = AF_INET;

    if((sd = socket(AF_INET,SOCK_STREAM,0)) != SOCKET_FAILED)
    {
        /* printf("co sd ok (%d)\n",sd); */
    }
    else
    {
        perror(NULL);
        return FALSE;
    }

/* connect */

    if(sd == INVALID_SOCKET)
    {
        return FALSE;
    }

    if(strcmp(machine,"nobody")==0)
        gethostname(machine,80);

    if(port == 0)
        port = 4701;

    printf("sockets version attempting connection to machine : %s port : %d\n",machine,port);
    server = gethostbyname(machine);
    if(server == NULL) return FALSE;
    memset(&them,0,sizeof(them));
    memcpy((char *)&them.sin_addr,server->h_addr,server->h_length);
    them.sin_family = AF_INET;
    them.sin_port = htons(port);
    memcpy((char *)&m_them,&them,sizeof(m_them));

    if((connect(sd,(struct sockaddr *)&them,sizeof(them))) == CONNECT_FAILED)
    {
        perror(NULL);
        return FALSE;
    }


    sprintf(buffer,"xxx");

    ssize_t writeSoFar = 0;
    void * ptr = buffer;

    while (writeSoFar < REALTIME_BUFFER_SIZE)
    {
        ssize_t writeC = 0;

        /* += would complicate code */
        assert( writeSoFar < REALTIME_BUFFER_SIZE);
        if((writeC = write(sd,ptr,(size_t)(REALTIME_BUFFER_SIZE - writeSoFar))) == RASOCKERR)
        {
            return(FALSE);
        }
        writeSoFar = writeSoFar + writeC;
        ptr = (char *)ptr + writeC;

    }


    return (0);
}

