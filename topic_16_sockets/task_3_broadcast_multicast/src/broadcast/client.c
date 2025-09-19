#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define PORT 7777
#define SERVER_IP "255.255.255.255"
#define MAX_MSG 16

int main(void){
    char msg[MAX_MSG] = {0};
    struct sockaddr_in serv, sender_addr;

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

    int flag = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
        perror("setsockopt reuse failed");
        exit(EXIT_FAILURE);
    }

    if(bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed bind");
        exit(EXIT_FAILURE);
    }

    socklen_t sender_len = sizeof(sender_addr);

    if(recvfrom(fd, msg, MAX_MSG, 0, (struct sockaddr*)&sender_addr, &sender_len) == -1){
        perror("failed recvfrom");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", msg);

    return 0;
}