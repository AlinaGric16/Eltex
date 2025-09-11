#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_MSG 10
#define PORT 8080

int main(void){
    struct sockaddr_in serv, cli;
    socklen_t struct_cli_len = sizeof(cli);
    char buf[MAX_MSG];
    size_t size = MAX_MSG;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);

    memset(&cli, 0, sizeof(cli));

    if(bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recvfrom(fd, &buf, size, 0, (struct sockaddr*)&cli, &struct_cli_len) == -1){
        perror("failed recvfrom");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);
    strncpy(buf, "Hi!", MAX_MSG);

    if(sendto(fd, &buf, strlen(buf)+1, 0, (struct sockaddr*)&cli, struct_cli_len) == -1){
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }


    close(fd);
    return 0;
}