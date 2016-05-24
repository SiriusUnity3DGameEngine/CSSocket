#include "common.h"
#include <netdb.h>

int connectToServer(char *host, int port);

int main(int argc, char *argv[]){
    int server;
    char buffer[256];

    server= connectToServer("localhost", 5000);

    printf("Mesajinizi girin: ");
    fgets(buffer,255,stdin);

    //servera gonder
    write(server,buffer,strlen(buffer));

    //okuma yapmatan once buffer temizleniyor.
    //soketten ten \0 gonderilmeyebilir
    bzero(buffer,256);

    //serverdan okuma
    read(server,buffer,255);
    printf("%s\n",buffer);

    close(server);
    return 0;
}

int connectToServer(char *host, int port){
    int sockfd; // socket file desc

    struct sockaddr_in serverAdress;
    struct hostent *server;

    //soket file_desc kurulmasi
    //socket(domain, type, protocol)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //soketin basarili bir sekilde acilip acilmadigin kontrol et
    if (sockfd < 0)
        error("ERROR opening socket");

    //host adres bilgilerinin alinmasi
    server = gethostbyname(host);
    if (server == NULL)
        error("ERROR, no such host");

    //serverAdress temizleniyor
    bzero((char *) &serverAdress, sizeof(serverAdress));

    // serverAdress nin ayarlarinin yapilmasi
    serverAdress.sin_family = AF_INET; // internet domain/family (internet ipv4 icin AF_INET)
    serverAdress.sin_port = htons(port);  //htons byte order degistirmek icin kullanilir.
    bcopy((char *)server->h_addr, (char *)&serverAdress.sin_addr.s_addr, server->h_length); // server ip

    //servera baglanma
    if (connect(sockfd,(struct sockaddr *) &serverAdress,sizeof(serverAdress)) < 0)
        error("ERROR connecting");

    return sockfd;
}
