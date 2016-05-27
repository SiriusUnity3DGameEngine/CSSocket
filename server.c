#include "server.h"

int main(int argc, char *argv[]){
    Server* server;

    server= createTCPServer(5000);
    printf("Server kuruldu, client bekleniyor\n");

    server->OnNewClient= ElciThread;

    startSekreter(server);

    char c;
    scanf("%c", &c);

    closeServer(server);

    return 0;
}

Server* createTCPServer(int port){
    int sockfd; // socket file desc

    struct sockaddr_in serverAdress; // serverin adresinin temsili

    int socketReUse = 1;

    //soket file_desc kurulmasi
    //socket(domain, type, protocol)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //soketin basarili bir sekilde acilip acilmadigin kontrol et
    if (sockfd < 0)
        error("ERROR opening socket");

    //serverAdress nin icini temizle ( sifir ile doldurma)
    //bzero(memory_adress, size)
    bzero((char *) &serverAdress, sizeof(serverAdress));

    // serverAdress nin ayarlarinin yapilmasi
    serverAdress.sin_family = AF_INET; // internet domain/family (internet ipv4 icin AF_INET)
    serverAdress.sin_port = htons(port); //htons byte order degistirmek icin kullanilir.
    serverAdress.sin_addr.s_addr = INADDR_ANY; //ip adresi. serverin bir ip girmesi gerekmez

    //soketi kapattiktan sonra hemen kullanima acilmasi.
    setsockopt(sockfd ,SOL_SOCKET,SO_REUSEADDR, &socketReUse, sizeof(int));

    //bind : socket file_desc ile server adresinin baglanmasi
    if (bind(sockfd, (struct sockaddr *) &serverAdress, sizeof(serverAdress)) < 0)
        error("ERROR on binding");

    //soketimizi dinlemye basla.
    //listen(socket, backlog)
    // backlog: baglanma sirasinin maksimum uzunlugu
    listen(sockfd, 5);

    Server* server= (Server*)malloc(sizeof(Server));
    server->mainFD= sockfd;

    server->isRunning= TRUE;

    server->lastClientID= 0;

    int i=0;
    for(; i<MAX_CLIENT; i++){
        server->conns[i]= NULL;
    }

    return server;
}

void startSekreter(Server* server){
    if(pthread_create( &server->sekreterThread, NULL, SekreterThread, (void*) server)!=0){
        perror("Sekreter thread");
    }
    debug("Sekreter başlatılıyor");
}

void closeServer(Server* server){
    server->isRunning= FALSE;
    close(server->mainFD);
}

ConnInfo* getConnInfo(Server* server, int clientID){
    int i=0;
    for(; i<MAX_CLIENT; i++){
        if(server->conns[i]!=NULL && server->conns[i]->clientID == clientID){
            return server->conns[i];
        }
    }
    return NULL;
}

void removeConnInfo(Server* server, int clientID){
    int i=0;
    for(; i<MAX_CLIENT; i++){
        if(server->conns[i]->clientID == clientID){
            server->conns[i]= NULL;
            return;
        }
    }
}

void addConnInfo(Server* server, ConnInfo* connInfo){
    int i=0;
    for(; i<MAX_CLIENT; i++){
        if(server->conns[i] == NULL){
            server->conns[i]= connInfo;
            return;
        }
    }
}

int acceptNewClient(int socketFd){
    int clientSocketFd;
    struct sockaddr_in clientAdress;
    int clientLength;

    clientLength = sizeof(clientAdress);

    // accept istedigimiz soketten yeni bir baglanti gelene kadar bekler.
    //accept(socket, address, addresslen)
    //not: addresslen pointer olarak verilmek zorunda,
    //      cunku degisime ugrayabilir. (bknz: value-result  parameter)
    clientSocketFd = accept(socketFd, (struct sockaddr *)&clientAdress, &clientLength);
    if (clientSocketFd < 0)
        error("ERROR on accept");

    return clientSocketFd;
}

void* SekreterThread( void* _server ){
    int client;
    Server* server= (Server*)_server;

    debug("Sekreter başlatıldı");

    while(server->isRunning){
        debug("Yeni client bekeniyor");
        client= acceptNewClient(server->mainFD);

        ConnInfo* connInfo= (ConnInfo*)malloc(sizeof(ConnInfo));
        connInfo->server= server;
        connInfo->clientFD= client;
        connInfo->clientID= server->lastClientID;

        addConnInfo(server, connInfo);

        if(pthread_create( &connInfo->thread, NULL, server->OnNewClient, (void*) connInfo)!=0){
            perror("Elçi thread başlatılamıyor");
        }
        debug("Elçi başlatılıyor");

        server->lastClientID++;

        debug("Yeni client: %d", connInfo->clientID);
    }
}

void* ElciThread (void* _connInfo){
    ConnInfo* connInfo= (ConnInfo*)_connInfo;

    Server* server= connInfo->server;
    int clientFD= connInfo->clientFD;
    int clientID= connInfo->clientID;

    Package* package;

    debug("Elci[%d] başlatıldı", clientID);

    // send client id
    sendPackageAndFree(clientFD, intPack(clientID, SERVER, clientID));

    while(server->isRunning){
        package= readPackage(clientFD);

        if(package==NULL)return;

        if( package->header.to != SERVER){
            // this means package is not belong to server

            //find real owner
            ConnInfo* to= getConnInfo(server, package->header.to);

            // try to transfer package to real owner
            if(to != NULL){
                //debug("Server transfering package to: %d", package->header.to);
                sendPackageAndFree(to->clientFD, package);
            }
            else{
                debug("Server cant transfer this package to: %d, size was: %d, type was: %d", package->header.to, package->header.size, package->header.type);
                sendPackageAndFree(clientFD, actionPack(CLIENT_NOT_EXIST, SERVER, clientID));
            }

            continue;
        }

        if( package->header.type == STRING){
            char* str= getStr(package);

            debug("New message from client: %s", str);

            char *save;
            char delim[] = " ";
            char* tok;

            tok= strtok_r(str, delim, &save);

            if(strcmp(tok, "list")==0 ){
                char buffer[2048];
                char buffer2[256];
                buffer[0]='\0';

                int i=0;
                for(; i<MAX_CLIENT; i++){
                    if(server->conns[i] != NULL){
                        sprintf(buffer2, "%sClient[%d]\n", (server->conns[i]->clientID==clientID ? "{You}":""), server->conns[i]->clientID );
                        strcat(buffer, buffer2);
                    }
                }
                // send client list to client as response
                sendPackageAndFree(clientFD, strPack(buffer, SERVER, clientID));
            }
            else{
                sendPackageAndFree(clientFD, strPack("Command unknown\n", SERVER, clientID));
            }
        }
    }

    close(clientFD);
}