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
static bool testMixedOperations();
static bool testMultipleOperations();

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
    result &= testMixedOperations();
    result &= testMultipleOperations();
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
    char * text = (char * )"test buffer";
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    char * buffer;
    int nBytes;
    bool result = getAndLockOTBForWrite(hBuffer, &buffer, strlen(text));
    assert(result);
    strcpy(buffer, text);
    unlockOTBForWrite(hBuffer,strlen(text) + 1);
    char * buffer2;
    size_t nBytes2;
    result = getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(strcmp(buffer, text) == 0);
    assert(nBytes2 == strlen(text) + 1);
    return result;
}

static bool testMixedOperations()
{
    char * text = (char * )"12345678901234567890";
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    char * buffer;
    int nBytes;
    bool result = getAndLockOTBForWrite(hBuffer, &buffer, strlen(text));
    assert(result);
    memcpy(buffer, text, 20);
    unlockOTBForWrite(hBuffer,20);
    char * buffer2;
    size_t nBytes2;
    result = getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(strncmp(buffer, text, 10) == 0);
    assert(nBytes2 == 20);
    unlockOTB(hBuffer, 10);
    result = getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    unlockOTB(hBuffer, 10);
    result = !getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    return result;
}

static bool testMultipleOperations()
{
    char * text = (char * )"12345678901234567890";
    char * text2 = (char * )"ABCDEFGHIJKLMNOPQRS";
    size_t sizeofText = strlen(text);
    size_t sizeofText2 = strlen(text2);
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    char * buffer;
    int nBytes;
    bool result = getAndLockOTBForWrite(hBuffer, &buffer, strlen(text));
    memcpy(buffer, text, sizeofText);
    unlockOTBForWrite(hBuffer,20);
    char * buffer2;
    size_t nBytes2;
    int ii = 0;
    while (getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2))
    {
        assert( buffer2[0] == buffer2[ii]);
        unlockOTB(hBuffer, 1);
    }
    return result;
}