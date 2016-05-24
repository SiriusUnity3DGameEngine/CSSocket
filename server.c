#include "common.h"

int createTCPServer(int port);
int acceptNewClient(int socketFd);

int main(int argc, char *argv[]){
    int server, client;
    char buffer[256];

    server= createTCPServer(5000);
    printf("Server kuruldu, client bekleniyor\n");
    client= acceptNewClient(server);

    //okuma yapmatan once buffer temizleniyor.
    //soketten ten \0 gonderilmeyebilir
    bzero(buffer,256);
    read(client,buffer,255);
    printf("Gelen mesaj: %s\n",buffer);

    //client e geri mesaj yollama
    write(client,"Mesajini aldim",14);

    //server ve client fd lerinin kapatilmasi
    close(server);
    close(client);
    return 0;
}

int createTCPServer(int port){
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

    return sockfd;
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
