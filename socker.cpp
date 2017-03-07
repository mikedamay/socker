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
#include <assert.h>
#include "rasocket.h"
#include "utils.h"
#include "send.h"
#include "echo.h"
#include "proxy.h"
#include "testOneTimeBuffer.h"

static const int USAGE_LEN = 1024;
static const int NUM_ACTIONS = 4;
enum {NO_ACTION, SUCCESS, FAILURE};

int main(int argc, char **argv)
{
    char usages[NUM_ACTIONS][USAGE_LEN];
    int actionIdx = 0;
    int result = NO_ACTION;
    unsigned short port = 0;
    char machine[ADDRESS_SIZE];

    WINSOCK_START
    if (!getHostAndPort(argc, argv, ARG_REMOTE_HOST, ARG_PORT, machine, ADDRESS_SIZE, &port))
    {
        exit(255);
    }

    if (ssend_usage(argc, argv, usages[actionIdx++], USAGE_LEN))
    {
        result = ssend(machine, port) ? SUCCESS : FAILURE;
    }
    else if (secho_usage(argc, argv, usages[actionIdx++], USAGE_LEN))
    {
        result = secho(machine, port) ? SUCCESS : FAILURE;
    }
    else if (sproxy_usage(argc, argv, usages[actionIdx++], USAGE_LEN))
    {
        result = sproxy(argc, argv) ? SUCCESS : FAILURE;
    }
    else if (testOneTimeBuffer_usage(argc, argv, usages[actionIdx++], USAGE_LEN))
    {
        result = testOneTimeBuffer(argc, argv) ? SUCCESS : FAILURE;
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
