#ifndef __SERVER_H__
#define __SERVER_H__

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define MSG_REGISTER 1
#define MSG_TEXT     2
#define MSG_LEAVE    3
#define MSG_SYSTEM   10

#define MAX_CLIENTS 5
#define MAX_PENDING 5
#define MAX_HISTORY 20

struct chat_message{
    int mtype;
    pid_t client_pid;
    char client_name[32];
    char text[256];
};

struct message_buffer {
    struct chat_message messages[MAX_PENDING];
    int head, tail;
    pthread_mutex_t lock;
};

struct client_info {
    pid_t pid;
    char queue_name[64];
    mqd_t queue_fd;
    char name[32];
    int is_active;
};

extern struct mq_attr attr;
extern struct message_buffer buffer;

extern struct chat_message message_history[MAX_HISTORY];
extern int history_count;

extern struct client_info clients[MAX_CLIENTS];
extern int client_count;
extern int server_runing;

void register_client(pid_t pid, char *name);
void meiling(struct chat_message msg);
void deleted_client(pid_t pid);

void* meiling_thread(void* args);

#endif