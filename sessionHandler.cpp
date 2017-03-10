//
// Created by mike on 3/9/17.
//

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "rasocket.h"
#include "oneTimeBuffer.h"
#include "sessionHandler.h"

#define GOOD_MAGIC 48830        // BEBE
#define BAD_MAGIC 56026         // DADA


struct SessionIter
{
    struct SessionHandler * handler;
    int idx;
};

struct SessionHandler
{
    int magic;
    struct Session * sessions[1024];
    int numSessions;
    int maxSD;
    struct SessionIter iter;
};

struct Session
{
    int magic;
    SOCKET sdClient;
    SOCKET sdProxied;
    ONE_TIME_BUFFER_HANDLE fromClientBuffer;
    ONE_TIME_BUFFER_HANDLE toClientBuffer;
};

static void freeSessionHandler(struct SessionHandler *p);
static struct SessionHandler *callocSessionHandler();
static struct Session *callocSession();
static int getFreeSlot(struct SessionHandler * sh);
static int clientSD2Idx(struct SessionHandler * sh, SOCKET sdClient);
static void resetSummaryData(struct SessionHandler * sh);

SESSION_HANDLER_HANDLE createSessionHandler()
{
    struct SessionHandler * shh = callocSessionHandler();
    shh->maxSD = -1;
    return shh;
}

void destroySessionHandler(SESSION_HANDLER_HANDLE sh)
{
    freeSessionHandler((struct SessionHandler * )sh);
}

bool addSession(SESSION_HANDLER_HANDLE shh, SOCKET sdClient, SOCKET sdProxied
  , ONE_TIME_BUFFER_HANDLE hFromClient, ONE_TIME_BUFFER_HANDLE hToClient)
{
    struct SessionHandler * sh = (struct SessionHandler * )shh;
    struct Session * session = callocSession();
    if (session == NULL)
    {
        return false;
    }
    session->sdClient = sdClient;
    session->sdProxied = sdProxied;
    session->fromClientBuffer = hFromClient;
    session->toClientBuffer = hToClient;
    int idx = getFreeSlot(sh);
    sh->sessions[idx] = session;
    sh->numSessions = max(sh->numSessions, idx + 1);
    sh->maxSD = max(sh->maxSD, sdClient);
    sh->maxSD = max(sh->maxSD, sdProxied);
    return true;
}

void removeSession(SESSION_HANDLER_HANDLE shh, SOCKET sdClient)
{
    struct SessionHandler * sh = (struct SessionHandler * )shh;
    int idx = clientSD2Idx(sh, sdClient);
    assert(idx >= 0 && idx < sizeof sh->sessions / sizeof sh->sessions[0]);
    struct Session * session = sh->sessions[idx];
    sh->sessions[idx] = NULL;
    resetSummaryData(sh);
}

SESSION_ITER getSessionIter(SESSION_HANDLER_HANDLE shh)
{
    struct SessionIter * si = &((struct SessionHandler * )shh)->iter;
    si->idx = 0;
    return si;
}

bool getNextSession(SESSION_ITER hIter, SOCKET * psdClient, SOCKET * psdProxied
, ONE_TIME_BUFFER_HANDLE *pFromClient, ONE_TIME_BUFFER_HANDLE *pToClient)
{
    struct SessionIter * iter = (struct SessionIter * )hIter;
    struct SessionHandler * sh = iter->handler;
    if (iter->idx >= sizeof sh->sessions / sizeof sh->sessions[0])
    {
        return false;
    }
    do
    {
        if (sh->sessions[iter->idx] != NULL)
        {
            struct Session * session = sh->sessions[iter->idx];
            *psdClient = session->sdClient;
            *psdProxied = session->sdProxied;
            *pFromClient = session->fromClientBuffer;
            *pToClient = session->toClientBuffer;
            (iter->idx)++;
            return true;
        }
        (iter->idx)++;
    } while (iter->idx < sizeof sh->sessions / sizeof sh->sessions[0]);
    return false;
}
int getMaxSD(SESSION_HANDLER_HANDLE shh)
{
    return ((struct SessionHandler * )shh)->maxSD;
}

static int getFreeSlot(struct SessionHandler * sh)
{
    int idx = -1;
    int ii;
    for (ii = 0; ii < sizeof sh->sessions / sizeof sh->sessions[0]; ii++)
    {
        if (sh->sessions[ii] == 0)
        {
            idx = ii;
            break;
        }
    }
    assert(idx < sizeof sh->sessions / sizeof sh->sessions[0]);
    return idx;
}
static int clientSD2Idx(struct SessionHandler * sh, SOCKET sdClient)
{
    int idx = -1;
    int ii;
    for (ii = 0; ii < sizeof sh->sessions / sizeof sh->sessions[0]; ii++)
    {
        if (sh->sessions[ii] != NULL && sh->sessions[ii]->sdClient == sdClient )
        {
            idx = ii;
            break;
        }
    }
    assert(idx < sizeof sh->sessions / sizeof sh->sessions[0]);
    return idx;
}
static void resetSummaryData(struct SessionHandler * sh)
{
    sh->maxSD = -1;
    sh->numSessions = 0;
    int ii;
    for (ii = 0; ii < sizeof sh->sessions / sizeof sh->sessions[0]; ii++)
    {
        if (sh->sessions[ii] != NULL)
        {
            sh->numSessions = ii + 1;
            sh->maxSD = max(sh->maxSD, max(sh->sessions[ii]->sdClient, sh->sessions[ii]->sdProxied));
        }
    }
}

static struct Session * callocSession()
{
    struct Session * session = (struct Session *)calloc(1, sizeof (struct Session));
    if (session != NULL)
    {
        session->magic = GOOD_MAGIC;
    }
    return session;
}

static struct SessionHandler * callocSessionHandler()
{
    struct SessionHandler * sh = (struct SessionHandler *)calloc(1, sizeof (struct SessionHandler));
    if (sh != NULL)
    {
        sh->magic = GOOD_MAGIC;
        sh->iter.handler = sh;
        return sh;
    }
    return NULL;
}

static void freeSessionHandler(struct SessionHandler * p)
{
    if (p == NULL)
    {
        return;
    }
    p->magic = BAD_MAGIC;
    free(p);
}
