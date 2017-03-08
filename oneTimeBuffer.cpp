/**
 * The one time buffer is used by one component to write data and another to read it.
 *
 */


#include <cstring>
#include <assert.h>
#include "stdlib.h"
#include "oneTimeBuffer.h"

#define GOOD_MAGIC 43962    /* ABBA */
#define BAD_MAGIC 47789     /* BAAD */
#define GOOD_ITEM_MAGIC 65261 /* FEED */
#define BAD_ITEM_MAGIC 57007  /* DEAF */
#define OTB_BUFFER_SIZE 1024
#define MAGIC_LENGTH 4

struct OneTimeBufferItem {
    char * buffer;
    size_t bufferSize;
    size_t readOffwet;
    size_t bytesWritten;
    OneTimeBufferItem * next;
    OneTimeBufferItem * previous;
};

struct OneTimeBuffer {
    int magic;
    char * name;
    OneTimeBufferItem * bufferItemListNchor;
    OneTimeBufferItem * bufferItemListLead;
    bool lockedForRead;
    bool lockedForWrite;
};
static struct OneTimeBuffer * callocOneTimeBuffer(char * name);
static bool isValidBuffer(OneTimeBuffer * p);
static OneTimeBufferItem * callocOneTimeBufferItem(size_t length);
static void freeOneTimeBufferItem(OneTimeBufferItem * otbi);

ONE_TIME_BUFFER_HANDLE createOneTimeBuffer(char *name)
{
    return (ONE_TIME_BUFFER_HANDLE)callocOneTimeBuffer(name);
}

bool getAndLockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, char **pbuffer, size_t bufferLength)
{
    OneTimeBuffer * potb = (OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert (!potb->lockedForWrite);
    OneTimeBufferItem * pItem = callocOneTimeBufferItem(bufferLength + 2 * MAGIC_LENGTH);
    if (pItem == NULL)
    {
        return false;
    }
    pItem->next = potb->bufferItemListNchor;
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

void unlockOTBForWrite(ONE_TIME_BUFFER_HANDLE hBuffer, size_t bytesWritten)
{
    OneTimeBuffer * potb = (OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    assert (potb->lockedForWrite);
    potb->bufferItemListNchor->bytesWritten = bytesWritten;
    potb->lockedForWrite = false;
}

bool getAndLockOTBForRead(ONE_TIME_BUFFER_HANDLE hBuffer, char ** buffer, size_t * pnBytesToRead)
{
    OneTimeBuffer * potb = (OneTimeBuffer *)hBuffer;
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

void unlockOTB(ONE_TIME_BUFFER_HANDLE hBuffer, size_t nBytesRead)
{
    OneTimeBuffer * potb = (OneTimeBuffer *)hBuffer;
    assert(isValidBuffer(potb));
    if (!potb->lockedForRead)
    {
        return;
    }
    assert (!potb->lockedForWrite);
    potb->lockedForRead = false;
    potb->bufferItemListLead->readOffwet += nBytesRead;
    if (potb->bufferItemListLead->readOffwet >= potb->bufferItemListLead->bytesWritten)
    {   // discard this part of the one time buffer as it has now been completely read
        OneTimeBufferItem * otbi = potb->bufferItemListLead;
        potb->bufferItemListLead = potb->bufferItemListLead->previous;
        if ( potb->bufferItemListLead != NULL)
        {
            potb->bufferItemListLead->next = NULL;
        }
        freeOneTimeBufferItem(otbi);
    }
}
/**
 *
 * @param name to aid future diagnostics
 * @return pointer to one time buffer or null if insufficient memory
 */
static struct OneTimeBuffer * callocOneTimeBuffer(char * name)
{
    OneTimeBuffer * otb = (OneTimeBuffer *)calloc(1, sizeof (struct OneTimeBuffer));
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

static void freeOneTimeBuffer(OneTimeBuffer * p)
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

static bool isValidBuffer(OneTimeBuffer * p)
{
    return p->magic == GOOD_MAGIC;
}

static OneTimeBufferItem * callocOneTimeBufferItem(size_t length)
{
    OneTimeBufferItem * otbi = (OneTimeBufferItem *)calloc(1, sizeof (struct OneTimeBufferItem));
    if (otbi != NULL)
    {
        if ((otbi->buffer = (char *)calloc(1, length) ) != NULL)
        {
            *(int *)otbi->buffer = GOOD_ITEM_MAGIC;
            *(int *)(otbi->buffer +length - 4) = GOOD_ITEM_MAGIC;
            otbi->bufferSize = length;
            return otbi;
        }
    }
    return NULL;
}

static void freeOneTimeBufferItem(OneTimeBufferItem * otbi)
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
