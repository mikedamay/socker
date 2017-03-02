/*
    $Workfile: rasocket.h $

    $Date: 23/03/99 16:08 $        $Author: Tim $

    created by xxx   DD/MM/YY

    $Archive: /rthist/rasocket.h $
    $Revision: 2 $
*/
#ifndef _RASOCK_H
#define _RASOCK_H
#if 0
#ifdef MAINWIN
#include <windows.h>
#endif
#endif
#if defined (_WINDOWS) || defined (_WIN32)
#include <winsock.h>
#include <io.h>
#define MAXHOSTNAMELEN 80
#else
#if !defined (AIX) && !defined (ALPHA)
#include <syscall.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#if !defined (SUNOS41) && !defined (_WINDOWS) && !defined (_WIN32) && !defined (AIX)
#include <sys/socketvar.h>
#endif
// system call function prototypes
#ifdef GCC
extern "C" {
int socket(int,int,int);
int listen(int,int);
int connect(int,struct sockaddr *,int);
int bind(int,struct sockaddr *,int);
int accept(int,struct sockaddr *,int*);
int getsockopt(int,int,int,char *,int *);
int setsockopt(int,int,int,char *,int);
int ioctl(int,unsigned long,char *);
int gethostname(char *,int);
int select(int c,fd_set *a,fd_set *b,fd_set *c,struct timeval *);
int sendto(int,char*,int,int,struct sockaddr*,int);
int recvfrom(int,char*,int,int,struct sockaddr*,int*);
void bzero(char *,int);
}
#endif
#if !defined (_WIN32) && !defined (_WINDOWS)
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#if !defined (ALPHA) && !defined (AIX)
//#include <sys/sockio.h>
#endif
#include <net/if.h>
#endif
#if !defined (ALPHA) && !defined (_WIN32) && !defined (_WINDOWS) && !defined (AIX)
//#include <sys/filio.h>
#endif
#if !defined (_WINDOWS) && !defined (_WIN32)
#include <sys/param.h>
#endif
#endif
#if defined (_WINDOWS)
#define SOCKET_FAILED INVALID_SOCKET
#define CONNECT_FAILED SOCKET_ERROR
#else
#define SOCKET_FAILED -1
#define CONNECT_FAILED -1
/**
 * this is a bit tricky - confusion doubtless caused by my out-of-date experience.
 *
 * the intention is to be able to build with a C compiler (outside of CLion)
 * as well as the latest and greatest C++ in CLion.
 * Our standard build is with c++ 11.  The built-in manifest constant for
 * c++11 is #define __STDC_VERSION__  201112L.  The condition __STDC_VERSION__ < 201112L is sufficient
 * to allow us to build with C99, However CLion build does not seem to respect this condition (a mystery)
 * so we have to add a check for the definition of __STDC_VERSION__.  To build
 * without any standard we include a check for __GNUC_GNU_INLINE__.
 * Using the more intuitive __STRICT_ANSI__ did not work for the CLion c++11 build
 * The fact that CLion thinks the section below is active but the compiler treats it
 * (correctly) as in active is a bit disturbing.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ < 201112L || defined(__GNUC_GNU_INLINE__)
typedef int bool;
#define true 1
#define false 0
#endif



#endif
