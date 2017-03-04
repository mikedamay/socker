#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32) 
#include <sys/un.h>
#endif
#include <assert.h>
#include "rasocket.h"
#include "bind.h"
#include "usage.h"


static void ssleep(int seconds)
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);

}

static SOCKET sd;

bool sbind_usage(int argc, char **argv, char * usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"bind"
      ,(char *)"socker <action> <machinename> <portno>\n\te.g. socker bind localhost 80\n"
      , usageStr, usageLen);
}

bool sbind(const char * host, unsigned short port)
{
	int iResult;
	WSAData wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printError("socker:bind() WSAStartup failed");
		return false;
	}

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
	{
		printError(NULL);
		return false;
	}
	printf("socker:bind() socket created\n");
	struct hostent * server = gethostbyname(host);
	if (server == NULL)
	{
		printError(NULL);
		return false;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	memcpy(&sin.sin_addr.s_addr, server->h_addr_list[0], (unsigned short)server->h_length);
	memset(&sin.sin_zero, 0, sizeof sin.sin_zero);
	printf("socker:bind() about to bind on port\n");
	if (bind(sd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == BIND_FAILED)
	{
		printError(NULL);
		return false;
	}

    // listen for connections and echo messages back to the peer
    while (true)
    {
        printf("socket:bind()  about to call listen()\n");
        if (listen(sd, 10) == SOCKET_FAILED)
        {
			printError("listen failed:");
            return false;
        }
        else
        {
            SOCKET sd2;
            printf("socker:bind() about to call accept()\n");
            if ((sd2 = accept(sd, NULL, NULL)) == SOCKET_FAILED)
            {
				printError("accept failed:");
                return false;
            }
            char buffer[1024];
            printf("socker:bind() about to call read()\n");
            ssize_t numBytes;
            do
            {
                numBytes = recv(sd2, buffer, sizeof(buffer), 0);
                if (numBytes == -1)
                {
					printError("socker:bind() read failed:");
                    break;
                }
                else if (numBytes > 0)
                {
                    printf("socker:bind() about to print buffer\n");
                    printf(buffer);
                    printf("\n");
                    send(sd2, buffer, sizeof buffer, 0);
                }
                else
                {
                    printf("socker:bind() read of zero bytes performed\n");
					printError("socker:bind() zero bytes read error:");
                    break;
                }
            }
            while (true);   // while reading a message
            close(sd2);
        }   // while listening
    }
}