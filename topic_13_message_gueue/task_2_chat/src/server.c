#include "server.h"

struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(struct chat_message)
};

struct message_buffer buffer = {
    .head = 0,
    .tail = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER
};

struct chat_message message_history[MAX_HISTORY];
int history_count = 0;

struct client_info clients[MAX_CLIENTS] = {0};
int client_count = 0;

int server_runing = 1;

int main(void){
    mqd_t fd = mq_open("/chat_message", O_CREAT | O_RDWR, 0666, &attr);
    if(fd == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Нажмите Ctrl+C для остановки.\n");

    pthread_t broadcast_thread;
    pthread_create(&broadcast_thread, NULL, meiling_thread, &buffer);

    while(1){ 
        struct chat_message msg;
        mq_receive(fd, (char*)&msg, sizeof(msg), NULL);
        switch (msg.mtype)
        {
        case MSG_REGISTER:
            register_client(msg.client_pid, msg.client_name);
            break;
        case MSG_TEXT:
            
            meiling(msg);
            break;
        case MSG_LEAVE:
            deleted_client(msg.client_pid);
            break;
        }
    }

    printf("Работа сервера окончена.\n");
    server_runing = 0;
    mq_close(fd);
    mq_unlink("/chat_message");
    pthread_join(broadcast_thread, NULL);

    return 0;
}
