#include "server.h"

int main(void){
    struct sockaddr_in serv, cli;
    socklen_t struct_cli_len = sizeof(cli);
    char buf[MAX_MSG];

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);

    if(bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recvfrom(fd, &buf, MAX_MSG, 0, (struct sockaddr*)&cli, &struct_cli_len) == -1){
        perror("failed recvfrom");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);
    if(strlen(buf) >= 2){
        buf[1] = 'A';
    }
    printf("%s\n", buf);

    if(sendto(fd, &buf, strlen(buf)+1, 0, (struct sockaddr*)&cli, struct_cli_len) == -1){
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}