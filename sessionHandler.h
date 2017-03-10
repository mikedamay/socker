//
// Created by mike on 3/9/17.
//

#ifndef SOCKER_SESSIONHANDLER_H
#define SOCKER_SESSIONHANDLER_H
typedef void * SESSION_HANDLER_HANDLE;
#define SESSION_HANDLER_HANDLE_ERROR ((void *)0)
typedef void * SESSION_ITER;
#define SESSION_ITER_ERROR ((void *)0)
SESSION_HANDLER_HANDLE createSessionHandler();
void destroySessionHandler(SESSION_HANDLER_HANDLE sh);
bool addSession(SESSION_HANDLER_HANDLE shh, SOCKET sdClient, SOCKET sdProxied
  , ONE_TIME_BUFFER_HANDLE hFromClient, ONE_TIME_BUFFER_HANDLE hToClient);
int getMaxSD(SESSION_HANDLER_HANDLE shh);
void removeSession(SESSION_HANDLER_HANDLE shh, SOCKET sdClient);
SESSION_ITER getSessionIter(SESSION_HANDLER_HANDLE sh);
bool getNextSession(SESSION_ITER hIter, SOCKET * psdClient, SOCKET * psdProxied
, ONE_TIME_BUFFER_HANDLE *pFromClient, ONE_TIME_BUFFER_HANDLE *pToClient);
#endif //SOCKER_SESSIONHANDLER_H
