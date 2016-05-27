#ifndef CS_SOCKET_SERVER
#define CS_SOCKET_SERVER

#include "common.h"

#define MAX_CLIENT 255


struct ConnInfo
{
	struct Server* server;
	int clientFD;
	int clientID;

	pthread_t thread;
};

struct Server{
	int mainFD;

	pthread_t sekreterThread;
	void* (*OnNewClient)(void *connInfo);

	int isRunning;

	int lastClientID;

	struct ConnInfo* conns[MAX_CLIENT];
};

typedef struct ConnInfo ConnInfo;
typedef struct Server Server;


Server* createTCPServer(int port);

void startSekreter(Server* server);

void closeServer(Server*);

ConnInfo* getConnInfo(Server* server, int clientID);
void addConnInfo(Server* server, ConnInfo* connInfo);
void removeConnInfo(Server* server, int clientID);

/* ftp spesific */
void* ElciThread (void* connInfo);

/* private funcs */
int acceptNewClient(int socketFd);
void* SekreterThread( void* server );

#endif