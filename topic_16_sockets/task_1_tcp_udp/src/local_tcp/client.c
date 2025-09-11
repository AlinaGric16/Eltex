#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MSG 10
#define SOCKET_PATH "/tmp/local_tcp"

int main(void){
    struct sockaddr_un serv;
    char buf[MAX_MSG] = "Hello!";
    size_t size = MAX_MSG;

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SOCKET_PATH, 108);

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