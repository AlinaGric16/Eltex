#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define MAX_MSG 32
#define PORT 7777

volatile sig_atomic_t server_running = 1;

typedef struct {
    struct sockaddr_in client_addr;
    socklen_t addr_len;
} ClientData;

void* send_time(void* arg);
static void sigint_handler();

int main(void) {
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    
    if (bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1) {
        perror("Main bind failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started\nTo stop Ctrl+C\n");
    
    while (server_running) {
        ClientData data;
        data.addr_len = sizeof(data.client_addr);

        char buf[MAX_MSG];
        
        if (recvfrom(fd, &buf, MAX_MSG - 1, 0, (struct sockaddr*)&data.client_addr, &data.addr_len) == -1) {
            if (errno != EINTR) {
                perror("recvfrom failed");
            }
            continue;
        }

        pthread_t thread;
        pthread_attr_t attr;
        
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        
        if (pthread_create(&thread, &attr, send_time, &data) != 0) {
            perror("pthread_create failed");
        }
        
        pthread_attr_destroy(&attr);
    }

    printf("Server shat down\n");
    close(fd);
    return 0;
}

void* send_time(void* arg) {
    ClientData data = *(ClientData*)arg;

    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("Thread socket failed");
        return NULL;
    }
    
    struct sockaddr_in thread_serv;
    thread_serv.sin_family = AF_INET;
    thread_serv.sin_addr.s_addr = INADDR_ANY;
    thread_serv.sin_port = htons(0);
    
    if (bind(client_fd, (struct sockaddr*)&thread_serv, sizeof(thread_serv)) == -1) {
        perror("Thread bind failed");
        close(client_fd);
        return NULL;
    }
    
    time_t now = time(NULL);
    char time_str[MAX_MSG];
    ctime_r(&now, time_str);
    
    if (sendto(client_fd, time_str, strlen(time_str) + 1, 0, (struct sockaddr*)&data.client_addr, data.addr_len) == -1) {
        perror("Thread sendto failed");
    }
    
    close(client_fd);
    return NULL;
}

static void sigint_handler()
{
    server_running = 0;
}
