#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>

#define MAX_MSG 32
#define PORT 7777
#define SERVERS 5

volatile int server_running = 1;

typedef struct {
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

    const char name[] = "/time_queue";

    mq_unlink(name);

    struct mq_attr attr = {
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(ClientData)
    };

    mqd_t fd_mq = mq_open(name, O_CREAT | O_RDWR, 0666, &attr);
    if(fd_mq == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    
    if (bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1) {
        perror("Main bind failed");
        mq_close(fd_mq);
        mq_unlink(name);
        close(fd);
        exit(EXIT_FAILURE);
    }

    pthread_t thread[SERVERS];
    ClientData client_data;
    client_data.addr_len = sizeof(client_data.client_addr);
    
    for (int i = 0; i < SERVERS; i++) {
        
        if(pthread_create(&thread[i], NULL, send_time, (void*)(long)fd_mq)== -1){
            perror("pthread_create failed");
            mq_close(fd_mq);
            mq_unlink(name);
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    sleep(1);
    printf("Server started\nTo stop Ctrl+C\n");
    
    while (server_running) {

        char buf[MAX_MSG];
        if (recvfrom(fd, &buf, MAX_MSG - 1, 0, (struct sockaddr*)&client_data.client_addr, &client_data.addr_len) == -1) {
            if (errno != EINTR) {
                perror("recvfrom failed");
            }
            continue;
        }

        if (mq_send(fd_mq, (const char*)&client_data, sizeof(client_data), 0) == -1) {
            perror("mq_send failed");
        }
        
    }

    for (int i = 0; i < SERVERS; i++) {
        ClientData stop_msg;
        mq_send(fd_mq, (const char*)&stop_msg, sizeof(stop_msg), 0);
    }
    for (int i = 0; i < SERVERS; i++) {
        pthread_join(thread[i], NULL);
    }
    
    printf("Server shat down\n");
    mq_close(fd_mq);
    mq_unlink(name);
    close(fd);
    return 0;
}

void* send_time(void* arg) {
    mqd_t fd = (mqd_t)(long)arg;
    ClientData data;

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

        if(mq_receive(fd, (char*)&data, sizeof(ClientData), NULL) == -1){
            perror("Thread faile mq_receive");
            close(client_fd);
            return NULL;
        }

        if(!server_running){
            break;
        }

        time_t now = time(NULL);
        char time_str[MAX_MSG];
        ctime_r(&now, time_str);
        
        if (sendto(client_fd, time_str, strlen(time_str) + 1, 0, (struct sockaddr*)&data.client_addr, data.addr_len) == -1) {
            perror("Thread sendto failed");
        }

    }

    close(client_fd);
    return NULL;
}

void signal_handler() {
    server_running = 0;
}