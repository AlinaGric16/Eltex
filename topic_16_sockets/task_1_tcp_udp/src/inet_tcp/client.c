#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_MSG 10
#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main(void){
    struct sockaddr_in serv;
    char buf[MAX_MSG] = "Hello!";
    size_t size = MAX_MSG;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &serv.sin_addr);
    serv.sin_port = htons(PORT);

    if(connect(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        perror("failed connect");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(send(fd, &buf, strlen(buf)+1, 0) == -1){
        perror("failed send");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recv(fd, &buf, size, 0) == -1){
        perror("failed recv");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);

    close(fd);
    return 0;
}