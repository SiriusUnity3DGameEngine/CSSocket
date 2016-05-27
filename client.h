#ifndef CS_SOCKET_CLIENT
#define CS_SOCKET_CLIENT

#include "common.h"
#include <netdb.h>

typedef struct{
	int fd;
	int id;

	int responseWaiting;
	char* lastResponse;

	char sendingFile[255];

	pthread_t thread;
} Client;

Client* connectToServer(char *host, int port);

void* ClientThread(void* );

#endif