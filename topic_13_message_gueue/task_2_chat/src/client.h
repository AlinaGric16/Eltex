#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

#define MSG_REGISTER 1
#define MSG_TEXT     2
#define MSG_LEAVE    3
#define MSG_SYSTEM   10

#define MAX_CLIENTS 5

struct chat_message{
    int mtype;
    pid_t client_pid;
    char client_name[32];
    char text[256];
};

extern WINDOW *msg_win, *clients_win, *input_win;

extern char clients[MAX_CLIENTS][32];
extern int msg_line;
extern int client_line;

void setup_windows();
void cleanup_windows();
void add_message(const char* text);
void refresh_client_win();
void del_client(char *name);
void* message_receiver(void* args);

#endif 