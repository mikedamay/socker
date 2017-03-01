/***********************************************************************

	killsock.c 
	a simple program to stop the history server

	V0.0	CMM

(C) Synergy Real-time Systems 1996
************************************************************************/

#include <cstdlib>
#include "connect.h"




int main(int argc, char **argv)
{
    if (!connect(argc, argv))
    {
        exit(-1);
    }
    else
    {
        exit(0);
    }
}

