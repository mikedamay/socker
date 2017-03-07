//
// Created by mike on 3/7/17.
//
#include <string.h>
#include <assert.h>
#include "rasocket.h"
#include "oneTimeBuffer.h"
#include "testOneTimeBuffer.h"


static bool testCreate();
static bool testGetAndLockForWrite();
static bool testGetAndLockForRead();

bool testOneTimeBuffer_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    usageStr[0] = '\0';
    if(argc > ARG_ACTION)
    {
        if(strcmp(argv[ARG_ACTION],"test") != 0)
        {
            strncpy(usageStr, "socker test <function>", usageLen);
            return false;
        }
    }
    return true;
}

bool testOneTimeBuffer(int argc, char **argv)
{
    int result = true;
    result &= testCreate();
    result &= testGetAndLockForWrite();
    result &= testGetAndLockForRead();
}

static bool testCreate()
{
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer((char *)"test buffer");
    bool result = hBuffer != ONE_TIME_BUFFER_HANDLE_ERROR;
    assert(result);
    return result;
}

static bool testGetAndLockForWrite()
{
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer((char *)"test buffer");
    char * buffer;
    int nBytes;
    bool result = getAndLockOTBForWrite(hBuffer, &buffer, strlen("test buffer") + 1);
    assert(result);
     return result;
}

static bool testGetAndLockForRead()
{
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer((char *)"test buffer");
    char * buffer;
    int nBytes;
    bool result = getAndLockOTBForWrite(hBuffer, &buffer, strlen("test buffer"));
    assert(result);
    strcpy(buffer, "test buffer");
    unlockOTBForWrite(hBuffer,strlen("test buffer") + 1);
    char * buffer2;
    size_t nBytes2;
    result = getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(strcmp(buffer, "test buffer") == 0);
    assert(nBytes2 == strlen("test buffer") + 1);
    return result;
}
