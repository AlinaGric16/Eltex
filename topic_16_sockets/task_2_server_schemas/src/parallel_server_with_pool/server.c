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
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#define MAX_MSG 32
#define PORT 7777
#define SERVERS 5

volatile int server_running = 1;

pthread_mutex_t thread_mutex[SERVERS] = {PTHREAD_MUTEX_INITIALIZER};
pthread_cond_t cond[SERVERS] = {PTHREAD_COND_INITIALIZER};

typedef struct {
    int server;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
} ClientData;

void* send_time(void* arg);
void signal_handler();

int main(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
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

    pthread_t thread[SERVERS];
    ClientData client_data[SERVERS];
    
    for (int i = 0; i < SERVERS; i++) {
        client_data[i].server = i;
        client_data[i].addr_len = sizeof(client_data[i].client_addr);
        if(pthread_create(&thread[i], NULL, send_time, (void*)&client_data[i])== -1){
            perror("pthread_create failed");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    sleep(1);
    printf("Server started\nTo stop Ctrl+C\n");
    
    int i = 0;
    while (server_running) {
        
        pthread_mutex_lock(&thread_mutex[i]);

        char buf[MAX_MSG];
        if (recvfrom(fd, &buf, MAX_MSG - 1, 0, (struct sockaddr*)&client_data[i].client_addr, &client_data[i].addr_len) == -1) {
            if (errno != EINTR) {
                perror("recvfrom failed");
            }
            pthread_mutex_unlock(&thread_mutex[i]);
            continue;
        }

        pthread_cond_signal(&cond[i]);
        pthread_mutex_unlock(&thread_mutex[i]);

        i = (i + 1) % SERVERS;
    }

    for (int i = 0; i < SERVERS; i++) {
        pthread_cond_signal(&cond[i]);
    }
    for (int i = 0; i < SERVERS; i++) {
        pthread_join(thread[i], NULL);
    }
    
    printf("Server shat down\n");
    close(fd);
    return 0;
}

void* send_time(void* arg) {
    ClientData* data = (ClientData*)arg;

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
    
    while (server_running)
    {
        pthread_mutex_lock(&thread_mutex[data->server]);
        pthread_cond_wait(&cond[data->server], &thread_mutex[data->server]);

        if (!server_running) {
            pthread_mutex_unlock(&thread_mutex[data->server]);
            continue;;
        }

        time_t now = time(NULL);
        char time_str[MAX_MSG];
        ctime_r(&now, time_str);
        
        if (sendto(client_fd, time_str, strlen(time_str) + 1, 0, (struct sockaddr*)&data->client_addr, data->addr_len) == -1) {
            perror("Thread sendto failed");
        }

        pthread_mutex_unlock(&thread_mutex[data->server]);
        sleep(1);
    }

    close(client_fd);
    return NULL;
}

void signal_handler() {
    server_running = 0;
}
