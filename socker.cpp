/***********************************************************************

	socker
	a simple program to stop the history server


 To build from command line
 /usr/bin/gcc -x c -Dunix -c socker.cpp usage.cpp connect.cpp bind.cpp
 /usr/bin/gcc -g socker.o -o socker
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rasocket.h"
#include "connect.h"
#include "bind.h"

static const int USAGE_LEN = 1024;
static const int NUM_ACTIONS = 2;
enum {NO_ACTION, SUCCESS, FAILURE};

int main(int argc, char **argv)
{
    char usages[NUM_ACTIONS][USAGE_LEN];
    int actionIdx = 0;
    int result = NO_ACTION;
    unsigned short port = 0;
    char machine[ADDRESS_SIZE];

    strcpy(machine,"");
    if (argc > ARG_REMOTE_HOST) {
        strcpy(machine,argv[ARG_REMOTE_HOST]);
    }

    if(argc > ARG_PORT) {
        int nn;
        nn = atoi(argv[ARG_PORT]);
        if ( nn < 1 || nn > USHRT_MAX) {
            fprintf(stderr, "port [%s] must be between 1 and %d", argv[ARG_PORT], USHRT_MAX);
            exit(-1);
        }
        port = (unsigned short)nn;
    }
    if(strcmp(machine,"")==0)
        gethostname(machine,ADDRESS_SIZE);

    if(port == 0)
        port = DEFAULT_PORT;

    if (sconnect_usage(argc, argv, usages[actionIdx++], USAGE_LEN ))
    {
        result = sconnect(machine, port) ? SUCCESS : FAILURE;
    }
    else if (sbind_usage(argc, argv, usages[actionIdx++], USAGE_LEN ))
    {
        result = sbind(machine, port) ? SUCCESS : FAILURE;
    }
    assert(actionIdx <= NUM_ACTIONS); // try to remember to up the number of actions
    if ( result == NO_ACTION)
    {
        printf("usage:\n");
        printf(usages[0]);
        printf("\n");
        int ii;     // max compatibility
        for (ii = 1; ii < NUM_ACTIONS; ii++)
        {
            printf("or\n");
            printf(usages[ii]);
            printf("\n");
        }
    }
    exit(result != SUCCESS);
}
