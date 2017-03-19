//
// Created by mike on 3/6/17.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rasocket.h"
#include "utils.h"

bool getHostAndPort(int argc, char ** argv, int posHostArg, int posPortArge, char * host
  ,size_t hostlen, unsigned short * pport)
{
    strcpy(host,"");
    if (argc > posHostArg) {
        strcpy(host,argv[posHostArg]);
    }

    if(argc > posPortArge) {
        int nn;
        nn = atoi(argv[posPortArge]);
        if ( nn < 1 || nn > USHRT_MAX) {
            fprintf(stderr, "port [%s] must be between 1 and %d", argv[posPortArge], USHRT_MAX);
            return false;
        }
        *pport = (unsigned short)nn;
    }
    if(strcmp(host,"")==0)
        gethostname(host,hostlen);

    if(*pport == 0)
        *pport = DEFAULT_PORT;
    return true;
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

