#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_MSG 32
#define PORT 7777
#define SERVER_IP "127.0.0.1"

int main(void) {
    struct sockaddr_in serv;
    char request[] = "time";
    char time_str[MAX_MSG];
    
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("Client socket failed");
        exit(EXIT_FAILURE);
    }
    
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv.sin_addr);
    
    if (sendto(fd, request, strlen(request) + 1, 0,
              (struct sockaddr*)&serv, sizeof(serv)) == -1) {
        perror("Client sendto failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in response_addr;
    socklen_t addr_len = sizeof(response_addr);
    
    if (recvfrom(fd, time_str, MAX_MSG - 1, 0, (struct sockaddr*)&response_addr, &addr_len) == -1) {
        perror("Client recvfrom failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    printf("%s", time_str);
    
    close(fd);
    return 0;
}