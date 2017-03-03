#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32) 
#include <sys/un.h>
#endif
#include <assert.h>
#include "rasocket.h"
#include "bind.h"
#include "usage.h"

static SOCKET sd;

bool sbind_usage(int argc, char **argv, char * usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"bind"
      ,(char *)"socker <action> <machinename> <portno>\n\te.g. socker bind localhost 80\n"
      , usageStr, usageLen);
}


bool sbind(const char * host, unsigned short port)
{
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
    {
        perror(NULL);
        return false;
    }
    struct hostent * server = gethostbyname(host);
    if (server == NULL)
    {
        perror(NULL);
        return false;
    }
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memcpy(&sin.sin_addr.s_addr, server->h_addr_list[0], (unsigned short)server->h_length);
    memset(&sin.sin_zero, 0, sizeof sin.sin_zero);
    if ( bind(sd, (struct sockaddr *)&sin, sizeof (struct sockaddr_in)) == BIND_FAILED)
    {
        perror(NULL);
        return false;
    }
	struct timeval tv;
	tv.tv_sec = 30;
	tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);
}