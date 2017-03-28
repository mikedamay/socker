
#include <string.h>
#include <stdio.h>
#include "rasocket.h"
#include "usage.h"

bool usage(int argc, char ** argv, char * action, char * usageText, char * usageStr, size_t usageLen)
{
    usageStr[0] = '\0';
    if(argc > ARG_ACTION)
    {
        if(strcmp(argv[ARG_ACTION],action) == 0)
        {
            return true;
        }
    }
    strncpy(usageStr, usageText, usageLen);
    return false;

}