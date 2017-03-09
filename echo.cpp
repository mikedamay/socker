#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(WIN32) 
#include <sys/un.h>
#endif
#include "rasocket.h"
#include "echo.h"
#include "usage.h"

static SOCKET sd;

bool secho_usage(int argc, char **argv, char *usageStr, size_t usageLen)
{
    return usage(argc, argv, (char *)"echo"
      ,(char *)"socker echo <machinename> <portno>\n\te.g. socker echo localhost 11900\n\tlistens on a port and sends any message received back to the client\n"
      , usageStr, usageLen);
}

bool secho(const char *host, unsigned short port)
{
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_FAILED)
	{
		printError(NULL);
		return false;
	}
	printf("socker:echo() socket created\n");
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
	printf("socker:echo() about to bind on port\n");
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
            printf("socker:echo() about to call accept()\n");
            if ((sd2 = accept(sd, NULL, NULL)) == SOCKET_FAILED)
            {
				printError("accept failed:");
                return false;
            }
            char buffer[1024];
            printf("socker:echo() about to call read()\n");
            ssize_t numBytes;
            do
            {
                numBytes = recv(sd2, buffer, sizeof(buffer), 0);
                if (numBytes == -1)
                {
					printError("socker:echo() read failed:");
                    break;
                }
                else if (numBytes > 0)
                {
                    printf("socker:echo() about to print buffer\n");
                    printf(buffer);
                    printf("\n");
                    send(sd2, buffer, sizeof buffer, 0);
                }
                else
                {
                    printf("socker:echo() read of zero bytes performed\n");
					printError("socker:echo() zero bytes read error:");
                    break;
                }
            }
            while (true);   // while reading a message
            close(sd2);
        }   // while listening
    }
}