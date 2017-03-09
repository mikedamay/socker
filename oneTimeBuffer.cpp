/**
 * The one time buffer is used by one component to write data and another to read it.
 *
 */


#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "rasocket.h"
#include "oneTimeBuffer.h"

#define GOOD_MAGIC 43962    /* ABBA */
#define BAD_MAGIC 47789     /* BAAD */
#define GOOD_ITEM_MAGIC 65261 /* FEED */
#define BAD_ITEM_MAGIC 57007  /* DEAF */
#define MAGIC_LENGTH 4

struct OneTimeBufferItem {
    char * buffer;
    ssize_t bufferSize;
    ssize_t readOffwet;
    ssize_t bytesWritten;
    struct OneTimeBufferItem * previous;
};

struct OneTimeBuffer {
    int magic;
    char * name;
    struct OneTimeBufferItem * bufferItemListNchor;
    struct OneTimeBufferItem * bufferItemListLead;
    bool lockedForRead;
    bool lockedForWrite;
    ssize_t availableBytes;
};
static struct OneTimeBuffer * callocOneTimeBuffer(char * name);
static bool isValidBuffer(struct OneTimeBuffer * p);
static struct OneTimeBufferItem * callocOneTimeBufferItem(ssize_t length);
static void freeOneTimeBufferItem(struct OneTimeBufferItem * otbi);
static bool getAndLockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, char **pbuffer, ssize_t bufferLength);
static void unlockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, ssize_t bytesWritten);
static void freeOneTimeBuffer(struct OneTimeBuffer * p);

ONE_TIME_BUFFER_HANDLE createOneTimeBuffer(char *name)
{
    return (ONE_TIME_BUFFER_HANDLE)callocOneTimeBuffer(name);
}

bool writeOTB(ONE_TIME_BUFFER_HANDLE hBuffer, char * buffer,  ssize_t nBytesWritten)
{
    char * otbBuffer;
    if (getAndLockOTBForWrite(hBuffer, &otbBuffer, nBytesWritten ))
    {
        assert(nBytesWritten >= 0 && nBytesWritten <= UINT_MAX);
        memcpy(otbBuffer, buffer, (size_t)nBytesWritten);
        unlockOTBForWrite(hBuffer, nBytesWritten);
        return true;
    }
    return false;
}

static bool getAndLockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, char **pbuffer, ssize_t bufferLength)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert (!potb->lockedForWrite);
    struct OneTimeBufferItem * pItem = callocOneTimeBufferItem(bufferLength + 2 * MAGIC_LENGTH);
    if (pItem == NULL)
    {
        return false;
    }
    if (potb->bufferItemListNchor != NULL)
    {
        potb->bufferItemListNchor->previous = pItem;
    }
    else
    {
        potb->bufferItemListLead = pItem;
    }
    potb->bufferItemListNchor = pItem;
    *pbuffer = (pItem->buffer + MAGIC_LENGTH);
    potb->lockedForWrite = true;
    return true;
}

void unlockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, ssize_t bytesWritten)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert (potb->lockedForWrite);
    potb->bufferItemListNchor->bytesWritten = bytesWritten;
    potb->availableBytes += bytesWritten;
    potb->lockedForWrite = false;
}

bool getAndLockOTBForRead(ONE_TIME_BUFFER_HANDLE hBuffer, char ** buffer, ssize_t * pnBytesToRead)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert (!potb->lockedForRead);
    assert (!potb->lockedForWrite);
    if (potb->bufferItemListLead == NULL)
    {
        return false;           // end of data reached
    }
    *buffer = potb->bufferItemListLead->buffer + MAGIC_LENGTH + potb->bufferItemListLead->readOffwet;
    *pnBytesToRead = potb->bufferItemListLead->bytesWritten - potb->bufferItemListLead->readOffwet;
    potb->lockedForRead = true;
    return true;
}

void unlockOTB(ONE_TIME_BUFFER_HANDLE hBuffer, ssize_t nBytesRead)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    if (!potb->lockedForRead)
    {
        return;
    }
    assert (!potb->lockedForWrite);
    potb->lockedForRead = false;
    potb->bufferItemListLead->readOffwet += nBytesRead;
    potb->availableBytes -= nBytesRead;
    if (potb->bufferItemListLead->readOffwet >= potb->bufferItemListLead->bytesWritten)
    {   // discard this part of the one time buffer as it has now been completely read
        struct OneTimeBufferItem * otbi = potb->bufferItemListLead;
        potb->bufferItemListLead = potb->bufferItemListLead->previous;
        if ( potb->bufferItemListLead == NULL)
        {
            assert(otbi == potb->bufferItemListNchor);
            potb->bufferItemListNchor = NULL;
        }
        freeOneTimeBufferItem(otbi);
    }
}

ssize_t availableBytesInOTB(ONE_TIME_BUFFER_HANDLE hBuffer)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert( potb->availableBytes > 0 || potb->availableBytes == 0 && potb->bufferItemListNchor == NULL
      && potb->bufferItemListLead == NULL);
    return potb->availableBytes;
}

void destroyOneTimeBuffer(ONE_TIME_BUFFER_HANDLE hBuffer)
{
    struct OneTimeBuffer * potb = (struct OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    freeOneTimeBuffer(potb);
}

/**
 *
 * @param name to aid future diagnostics
 * @return pointer to one time buffer or null if insufficient memory
 */
static struct OneTimeBuffer * callocOneTimeBuffer(char * name)
{
    struct OneTimeBuffer * otb = (struct OneTimeBuffer *)calloc(1, sizeof (struct OneTimeBuffer));
    if (otb != NULL)
    {
        otb->name = (char * )malloc(strlen(name) + 1);
        if (otb->name != NULL)
        {
            otb->magic = GOOD_MAGIC;
            strcpy(otb->name, name);
            return otb;
        }
        else
        {
            free(otb);
        }
    }
    return NULL;
}

static void freeOneTimeBuffer(struct OneTimeBuffer * p)
{
    if (p == NULL)
    {
        return;
    }
    if (p->name != NULL)
    {
        free(p->name);
        p->name = NULL;
    }
    p->magic = BAD_MAGIC;
    free(p);
}

static bool isValidBuffer(struct OneTimeBuffer * p)
{
    return p->magic == GOOD_MAGIC;
}

static struct OneTimeBufferItem * callocOneTimeBufferItem(ssize_t length)
{
    struct OneTimeBufferItem * otbi = (struct OneTimeBufferItem *)calloc(1, sizeof (struct OneTimeBufferItem));
    if (otbi != NULL)
    {
        assert(length >= 0 && length <= UINT_MAX);
        if ((otbi->buffer = (char *)calloc(1, (size_t)length) ) != NULL)
        {
            *(int *)otbi->buffer = GOOD_ITEM_MAGIC;
            *(int *)(otbi->buffer +length - 4) = GOOD_ITEM_MAGIC;
            otbi->bufferSize = length;
            return otbi;
        }
    }
    return NULL;
}

static void freeOneTimeBufferItem(struct OneTimeBufferItem * otbi)
{
    if (otbi == NULL)
    {
        return;
    }
    if ( otbi->buffer != NULL)
    {
        *(int *)otbi->buffer = BAD_ITEM_MAGIC;
        *(int *)(otbi->buffer + otbi->bufferSize - 4) = BAD_ITEM_MAGIC;

    }
    free(otbi);
}

