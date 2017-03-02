/***********************************************************************

	socker
	a simple program to stop the history server


 To build from command line
 /usr/bin/gcc   -x c -Dunix -D__USE_GNU -o socker.cpp.o -c socker.cpp
 /usr/bin/gcc  -x c -Dunix -D__USE_GNU -o connect.cpp.o -c connect.cpp
 /usr/bin/c++   -g   socker.cpp.o connect.cpp.o  -o socker
************************************************************************/

#include <stdlib.h>
#include "connect.h"




int main(int argc, char **argv)
{
    if (!sconnect(argc, argv))
    {
        exit(-1);
    }
    else
    {
        exit(0);
    }
}

