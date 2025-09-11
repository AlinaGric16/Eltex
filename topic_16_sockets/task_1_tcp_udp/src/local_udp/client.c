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
    char buf[MAX_MSG] = "Hello!";
    size_t size = MAX_MSG;

    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    cli.sun_family = AF_LOCAL;
    strncpy(cli.sun_path, CLIENT_PATH, sizeof(cli.sun_path) - 1);
    
    unlink(CLIENT_PATH);
    
    if (bind(fd, (struct sockaddr*)&cli, sizeof(cli)) == -1) {
        perror("failed client bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SERVER_PATH, sizeof(serv.sun_path) - 1);

    if (sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&serv, sizeof(serv)) == -1) {
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recv(fd, buf, size - 1, 0) == -1){
        perror("failed recv");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);

    close(fd);
    unlink(CLIENT_PATH);
    return 0;
}