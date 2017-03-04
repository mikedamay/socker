/***********************************************************************

	socker
	a simple program to stop the history server


 To build from command line
 /usr/bin/gcc -g -x c *.cpp -o socker
  noticeably the link line does not work if the files are suffixed .c (rather than .cpp)
 linker errors are undefined reference to sbind_usage and sbind.
 using nm on the .o files did not indicate the root cause.
 I assume that gcc behaves differently with .cpp vs. .c at the linker stage
 or maybe at the compile stage.  It could be an ordering issue of the.os
 I tried forcing bind.o first but not last.  Next step try that and
 find out the detailed behaviour of gcc.
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

    WINSOCK_START
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
