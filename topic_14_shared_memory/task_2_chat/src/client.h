#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ncurses.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define MAX_CLIENT_NAME 32
#define MAX_COUNT_CLIENTS 5
#define MAX_COUNT_MSG 50
#define MAX_LENGHT_MSG 100

extern struct sembuf post_sem;
extern struct sembuf wait_sem;

struct chat_room{
    int message_count;
    int user_count;
    char users[MAX_COUNT_CLIENTS][MAX_CLIENT_NAME];
    char history[MAX_COUNT_MSG][MAX_LENGHT_MSG];
    int client_semaphores[MAX_COUNT_CLIENTS];
};

struct info {
    int id_mem_access;
    int id_my_sem;
    int shm_id;
    int should_exit;
};

WINDOW *msg_win, *clients_win, *input_win;

extern int users;

void setup_windows();
void cleanup_windows();
void* message_receiver(void* args);
void refresh_client_win(struct chat_room *chat);
void add_message(struct chat_room *chat);
void del_client(struct chat_room *chat, char *client_name);
void clean(pthread_t receiver_thread, int my_sem, struct chat_room *chat);

#endif