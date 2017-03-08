//
// Created by mike on 3/7/17.
//

#ifndef SOCKER_ONETIMEBUFFER_H
#define SOCKER_ONETIMEBUFFER_H

typedef void * ONE_TIME_BUFFER_HANDLE;
#define ONE_TIME_BUFFER_HANDLE_ERROR ((void *)0)

ONE_TIME_BUFFER_HANDLE createOneTimeBuffer(char * name);
bool getAndLockOTBForRead(ONE_TIME_BUFFER_HANDLE hBuffer, char ** buffer, size_t * pnBytesToRead);
void unlockOTB(ONE_TIME_BUFFER_HANDLE hBuffer, size_t nBytesRead);
bool writeOTB(ONE_TIME_BUFFER_HANDLE hBuffer, char * buffer,  size_t nBytesWritten);
#endif //SOCKER_ONETIMEBUFFER_H
