//
// Created by mike on 3/9/17.
//

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rasocket.h"
#include "oneTimeBuffer.h"
#include "testSessionHandler.h"
#include "sessionHandler.h"

static bool testCreate();
static bool testAddSession();
static bool testRemoveSession();
static bool testRemoveMultipleSessions();
static bool testSessionIter();
static bool test1024Sessions();

bool testSessionHandler_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    usageStr[0] = '\0';
    if(argc > ARG_ACTION + 1)
    {
        if(strcmp(argv[ARG_ACTION],"test") == 0 && strcmp(argv[ARG_ACTION + 1], "sessionHandler") == 0)
        {
            return true;
        }
    }
    strncpy(usageStr, "socker test sessionHandler\n\trun self test", usageLen);
    return false;
}

bool testSessionHandler()
{
    int result = true;
    result &= testCreate();
    result &= testAddSession();
    result &= testRemoveSession();
    result &= testRemoveMultipleSessions();
    result &= testSessionIter();
    result &= test1024Sessions();
    return (bool)result;
}


static bool testCreate()
{
    printf((char *)"testCreate()\n");
    SESSION_HANDLER_HANDLE hHandler = createSessionHandler();
    assert(hHandler != SESSION_HANDLER_HANDLE_ERROR);
    destroySessionHandler(hHandler);
    return hHandler != NULL;
}

static bool testAddSession()
{
    printf((char *)"testAddSession()\n");
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    bool result = addSession(shh, 3, 4, NULL, NULL);
    int maxSD = getMaxSD(shh);
    assert(maxSD == 4);
    assert(result);
    return result;
}

static bool testRemoveSession()
{
    printf((char *)"testRemoveSession()\n");
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    addSession(shh, 3, 4, NULL, NULL);
    removeSession(shh, 3);
    assert(getMaxSD(shh) == -1);
    return getMaxSD(shh) == -1;
}

static bool testRemoveMultipleSessions()
{
    printf((char *)"testRemoveMultipleSessions()\n");
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    addSession(shh, 3, 4, NULL, NULL);
    assert(getMaxSD(shh) == 4);
    addSession(shh, 1, 2, NULL, NULL);
    assert(getMaxSD(shh) == 4);
    addSession(shh, 7, 8, NULL, NULL);
    assert(getMaxSD(shh) == 8);
    removeSession(shh, 1);
    assert(getMaxSD(shh) == 8);
    removeSession(shh, 7);
    assert(getMaxSD(shh) == 4);
    removeSession(shh, 3);
    assert(getMaxSD(shh) == -1);
    return getMaxSD(shh) == -1;
}

static bool testSessionIter()
{
    printf((char *)"testSessionIter()\n");
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    addSession(shh, 3, 4, NULL, NULL);
    addSession(shh, 1, 2, NULL, NULL);
    addSession(shh, 7, 8, NULL, NULL);
    SESSION_ITER iter = getSessionIter(shh);
    assert(iter != NULL);
    SOCKET sdClient, sdProxied;
    ONE_TIME_BUFFER_HANDLE hFromClient, hToClient;
    bool result;
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(result);
    assert(sdClient == 3);
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(result);
    assert(sdClient == 1);
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(result);
    assert(sdProxied == 8);
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(!result);
    removeSession(shh, 1);
    iter = getSessionIter(shh);
    getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(result);
    assert(sdClient == 7);
    removeSession(shh, 7);
    iter = getSessionIter(shh);
    getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    removeSession(shh, 3);
    iter = getSessionIter(shh);
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(!result);
    return iter != NULL;
}

static bool test1024Sessions()
{
    printf((char *)"test1024Sessions()\n");
    SESSION_HANDLER_HANDLE shh = createSessionHandler();
    int ii;
    for (ii = 0; ii < 1024; ii++)
    {
        addSession(shh, 3, 4, NULL, NULL);
    }
    SOCKET sdClient, sdProxied;
    ONE_TIME_BUFFER_HANDLE hFromClient, hToClient;
    bool result;
    SESSION_ITER iter = getSessionIter(shh);
    int jj;
    for (jj = 0; jj < 1024; jj++)
    {
        result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
        assert(result);
    }
    result = getNextSession(iter, &sdClient, &sdProxied, &hFromClient, &hToClient);
    assert(!result);
    return !result;
}