#ifndef CS_SOCKET_COMMON
#define CS_SOCKET_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

#define CHUNK_SIZE 1024

#define SERVER -1

#define DEBUG

#ifdef DEBUG
#define debug(args...)	printf(args); printf("\n")
#else
#define debug(args...)	/*no debug*/
#endif

typedef enum {INT, STRING, BYTE, ACTION} PackageType;
typedef enum {INCOMING_FILE, ACCEPT_FILE, CLIENT_NOT_EXIST} ActionType;

typedef struct {
	int from; // from and to shuold only used when sending client->server or client->client
	int to;
	int size; // size of data, not with headers
	PackageType type;
} Header;

typedef struct {
	Header header;
	char* data;
} Package;

Package* readPackage(int fd);
int sendPackage(int fd, Package* package);
int sendPackageAndFree(int fd, Package* package);

Package* newPackage(int size);
void freePackage(Package* package);

int getInt(Package* package);
char* getStr(Package* package);
ActionType getAction(Package* package);

Package* pack(const char* d, int size, PackageType type, int from, int to);
Package* intPack(int n, int from, int to);
Package* strPack(const char* str, int from, int to);
Package* actionPack(ActionType action, int from, int to);

#endif