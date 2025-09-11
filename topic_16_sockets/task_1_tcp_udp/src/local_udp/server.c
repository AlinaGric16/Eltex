#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MSG 10
#define SERVER_PATH "/tmp/local_udp_server"
#define CLIENT_PATH "/tmp/local_udp_client"


int main(void){
    struct sockaddr_un serv, cli;
    socklen_t struct_cli_len = sizeof(cli);
    char buf[MAX_MSG];
    size_t size = MAX_MSG;

    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SERVER_PATH, 108);

    unlink(SERVER_PATH);

    if(bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recv(fd, buf, size, 0) == -1){
        perror("failed recvfrom");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);
    strncpy(buf, "Hi!", MAX_MSG);

    cli.sun_family = AF_LOCAL;
    strncpy(cli.sun_path, CLIENT_PATH, 108);

    if(sendto(fd, &buf, strlen(buf)+1, 0, (struct sockaddr*)&cli, struct_cli_len) == -1){
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }


    unlink(SERVER_PATH);
    close(fd);
    return 0;
}