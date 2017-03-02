/***********************************************************************

	socker
	a simple program to stop the history server


 To build from command line
 /usr/bin/gcc   -x c -Dunix -D__USE_GNU -o socker.cpp.o -c socker.cpp
 /usr/bin/gcc  -x c -Dunix -D__USE_GNU -o connect.cpp.o -c connect.cpp
 /usr/bin/c++   -g   socker.cpp.o connect.cpp.o  -o socker
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "connect.h"

static const int USAGE_LEN = 1024;
static const int NUM_ACTIONS = 1;
enum {NO_ACTION, SUCCESS, FAILURE};

int main(int argc, char **argv)
{
    char usages[NUM_ACTIONS][USAGE_LEN];
    int actionIdx = 0;
    int result = NO_ACTION;
    if (sconnect_usage(argc, argv, usages[actionIdx++], USAGE_LEN ))
    {
        result = sconnect(argc, argv) ? SUCCESS : FAILURE;
    }
    if ( result == NO_ACTION)
    {
        printf("usage:\n");
        printf(usages[0]);
        printf("\n");
        for (int ii = 1; ii < NUM_ACTIONS; ii++)
        {
            printf("or\n");
            printf(usages[ii]);
            printf("\n");
        }
    }
    exit(result != SUCCESS);
}
