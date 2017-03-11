//
// Created by mike on 3/6/17.
//

#ifndef SOCKER_UTILS_H
#define SOCKER_UTILS_H
bool getHostAndPort(int argc, char ** argv, int posHostArg, int posPortArge, char * host
  ,size_t hostlen, unsigned short * port);
bool populateSockAddr(char * host, unsigned short port, struct sockaddr_in * psin);
#endif //SOCKER_UTILS_H
