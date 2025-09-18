#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define PORT 7777
#define MAX_MSG 32

volatile int server_running = 1;

typedef struct {
    struct sockaddr_in client_addr;
    socklen_t addr_len;
} ClientData;

void signal_handler();

int main(void){
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_tcp;

    int fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_tcp == -1){
        perror("failed sockaddr tcp");
        exit(EXIT_FAILURE);
    }

    serv_tcp.sin_family = AF_INET;
    serv_tcp.sin_addr.s_addr = INADDR_ANY;
    serv_tcp.sin_port = htons(PORT);

    int reuse = 1;
    if (setsockopt(fd_tcp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt TCP failed");
        close(fd_tcp);
        exit(EXIT_FAILURE);
    }

    if(bind(fd_tcp, (struct sockaddr*)&serv_tcp, sizeof(serv_tcp)) == -1){
        perror("failed bind tcp");
        close(fd_tcp);
        exit(EXIT_FAILURE);
    }

    if(listen(fd_tcp, 1) == -1){
        perror("failed listen");
        close(fd_tcp);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_udp;

    int fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd_udp == -1){
        perror("failed sockaddr udp");
        close(fd_tcp);
        exit(EXIT_FAILURE);
    }

    serv_udp.sin_family = AF_INET;
    serv_udp.sin_addr.s_addr = INADDR_ANY;
    serv_udp.sin_port = htons(PORT);

    if (setsockopt(fd_udp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt UDP failed");
        close(fd_tcp);
        close(fd_udp);
        exit(EXIT_FAILURE);
    }

    if(bind(fd_udp, (struct sockaddr*)&serv_udp, sizeof(serv_udp)) == -1){
        perror("failed bind udp");
        close(fd_tcp);
        close(fd_udp);
        exit(EXIT_FAILURE);
    }

    fd_set read_fds;
    int max_fd;

    FD_ZERO(&read_fds);
    FD_SET(fd_tcp, &read_fds);
    FD_SET(fd_udp, &read_fds);

    max_fd = (fd_tcp > fd_udp) ? fd_tcp : fd_udp;

    printf("Server started\nTo stop Ctrl+C\n");

    while (server_running) {
        char buf[MAX_MSG];
        fd_set temp_fds = read_fds;
        
        int ready = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if(ready == -1){
            if (errno != EINTR) {
                perror("failed select");
            }
            continue;
        }
        
        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &temp_fds)) {
                if(fd == fd_tcp){
                    struct sockaddr_in cli;
                    socklen_t cli_len = sizeof(cli);

                    int fd_cli = accept(fd, (struct sockaddr*)&cli, &cli_len);
                    if(fd_cli == -1){
                        perror("failed accept");
                        continue;
                    }

                    FD_SET(fd_cli, &read_fds);
                    if (fd_cli > max_fd) max_fd = fd_cli;
                }
                else if(fd == fd_udp){
                    struct sockaddr_in cli;
                    socklen_t cli_len = sizeof(cli);

                    if(recvfrom(fd, buf, MAX_MSG, 0, (struct sockaddr*)&cli, &cli_len) == -1){
                        perror("failed recvfrom");
                        continue;
                    }

                    time_t now = time(NULL);
                    char time_str[MAX_MSG];
                    ctime_r(&now, time_str);

                    if(sendto(fd, time_str, strlen(time_str)+1, 0, (struct sockaddr*)&cli, cli_len) == -1){
                        perror("failed sendto");
                    }
                }
                else{
                    if(recv(fd, buf, MAX_MSG, 0) == -1){
                        perror("failed recv");
                        FD_CLR(fd, &read_fds);
                        continue;
                    }

                    time_t now = time(NULL);
                    char time_str[MAX_MSG];
                    ctime_r(&now, time_str);

                    if(send(fd, time_str, strlen(time_str)+1, 0) == -1){
                        perror("failed send");
                    }

                    FD_CLR(fd, &read_fds);
                    close(fd);
                }
            }
        }
    }

    printf("Server shat down\n");

    close(fd_tcp);
    close(fd_udp);
    return(0);
}

void signal_handler() {
    server_running = 0;
}