#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MSG 10
#define PORT 8080

int main(void){
    struct sockaddr_in serv, cli;
    socklen_t struct_cli_len = sizeof(cli);
    char buf[MAX_MSG];
    size_t size = MAX_MSG;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
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

    if(listen(fd, 1) == -1){
        perror("failed listen");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int cli_fd = accept(fd, (struct sockaddr*)&cli, &struct_cli_len);
    if(cli_fd == -1){
        perror("failed accept");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recv(cli_fd, &buf, size, 0) == -1){
        perror("failed recv");
        close(cli_fd);
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);
    strncpy(buf, "Hi!", MAX_MSG);

    if(send(cli_fd, buf, strlen(buf)+1, 0) == -1){
        perror("failed send");
        close(cli_fd);
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(cli_fd);
    close(fd);
    return 0;
}