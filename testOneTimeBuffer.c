//
// Created by mike on 3/7/17.
//
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "rasocket.h"
#include "oneTimeBuffer.h"
#include "testOneTimeBuffer.h"


static bool testCreate();
static bool testGetAndLockForWrite();
static bool testGetAndLockForRead();
static bool testMixedOperations();
static bool testMultipleOperations();
static bool testMixedReadWriteOperations();

bool testOneTimeBuffer_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    usageStr[0] = '\0';
    if(argc > ARG_ACTION)
    {
        if(strcmp(argv[ARG_ACTION],"test") == 0)
        {
            return true;
        }
    }
    strncpy(usageStr, "socker test\n\trun self test", usageLen);
    return false;
}

bool testOneTimeBuffer()
{
    int result = true;
    result = result && testCreate();
    result = result && testGetAndLockForWrite();
    result = result && testGetAndLockForRead();
    result = result && testMixedOperations();
    result = result && testMultipleOperations();
    result = result && testMixedReadWriteOperations();
    return (bool)result;
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
    printf((char *)"testGetAndLockForWrite()\n");
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer((char *)"test buffer");
    bool result = writeOTB(hBuffer, (char * )"test buffer", strlen("test buffer") + 1);
    assert(result);
    return result;
}

static bool testGetAndLockForRead()
{
    printf("testGetAndLockForRead()\n");
    char * text = (char * )"test buffer";
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    bool result = writeOTB(hBuffer, text, strlen(text));
    assert(result);
    char * buffer2;
    ssize_t nBytes2;
    result = getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(strncmp(buffer2, text, 10) == 0);
    assert(nBytes2 == strlen(text));
    return result;
}

static bool testMixedOperations()
{
    printf("testMixedOperations()\n");
    char * text = (char * )"12345678901234567890";
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    bool result = writeOTB(hBuffer, text, strlen(text));
    assert(result);
    char * buffer2;
    ssize_t nBytes2;
    getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(strncmp(buffer2, text, 20) == 0);
    assert(nBytes2 == 20);
    unlockOTB(hBuffer, 10);
    getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    assert(nBytes2 == 10);
    unlockOTB(hBuffer, 10);
    result = !getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
    return result;
}

static bool testMultipleOperations()
{
    printf("testMultipleOperations()\n");
    char * text = (char * )"12345678901234567890";
    ssize_t sizeofText = strlen(text);
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    bool result = writeOTB(hBuffer, text, sizeofText);
    char * buffer2;
    ssize_t nBytes2;
    int ii = 0;
    while (getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2))
    {
        assert( buffer2[0] == text[ii]);
        unlockOTB(hBuffer, 1);
        ii++;
    }
    unlockOTB(hBuffer, 0);
    return result;
}

static bool testMixedReadWriteOperations()
{
    printf("testMixedReadWriteOperations()\n");
    char * text = (char * )"12345678901234567890";
    char * text2 = (char * )"ABCDEFGHIJKLMNOPQRS";
    ssize_t sizeofText = strlen(text);
    ssize_t sizeofText2 = strlen(text2);
    ONE_TIME_BUFFER_HANDLE hBuffer = createOneTimeBuffer(text);
    writeOTB(hBuffer, text, strlen(text));
    char * buffer2;
    ssize_t nBytes2;
    int ii = 0;
    for (ii = 0; ii < 3; ii++ )
    {
        getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
        assert( buffer2[0] == text[ii]);
        unlockOTB(hBuffer, 1);
    }
    bool result = writeOTB(hBuffer, text2, sizeofText2);
    ssize_t availableBytes = availableBytesInOTB(hBuffer);
    assert(availableBytes == sizeofText + sizeofText2 - 3);
    while (ii < sizeofText)
    {
        getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2);
        assert( buffer2[0] == text[ii]);
        unlockOTB(hBuffer, 1);
        ii++;
    }
    while (getAndLockOTBForRead(hBuffer, &buffer2, &nBytes2))
    {
        assert( buffer2[0] == text2[ii - sizeofText]);
        unlockOTB(hBuffer, 1);
        ii++;
    }
    assert( ii >= sizeofText + sizeofText2);
    destroyOneTimeBuffer(hBuffer);
    return result;
}