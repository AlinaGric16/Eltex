#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_CLIENT_NAME 32
#define MAX_COUNT_CLIENTS 5
#define MAX_COUNT_MSG 50
#define MAX_LENGHT_MSG 100

struct chat_room{
    int message_count;
    int user_count;
    char users[MAX_COUNT_CLIENTS][MAX_CLIENT_NAME];
    char history[MAX_COUNT_MSG][MAX_LENGHT_MSG];
    int client_semaphores[MAX_COUNT_CLIENTS];
};

void clear(int memory_access, int new_data,struct chat_room *chat, int chat_id, char *name);

#endif