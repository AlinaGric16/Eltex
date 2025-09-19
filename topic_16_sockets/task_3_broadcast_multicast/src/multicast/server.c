#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 7777
#define SERVER_IP "224.0.0.1"

int main(void){
    char msg[] = "Message";
    struct sockaddr_in serv;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &serv.sin_addr) <= 0){
        perror("failed inet_pton");
        exit(EXIT_FAILURE);
    }

    if(sendto(fd, msg, strlen(msg)+1, 0, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed sendto");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}