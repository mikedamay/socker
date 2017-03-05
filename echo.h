//
// Created by mike on 3/3/17.
//
#include <stdlib.h>
#ifndef SOCKER_BIND_H
#define SOCKER_BIND_H
bool secho_usage(int argc, char **argv, char *usageStr, size_t usageLen);
bool secho(const char *host, unsigned short port);
#endif //SOCKER_BIND_H
