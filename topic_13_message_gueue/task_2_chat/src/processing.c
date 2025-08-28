#include "server.h"

void meiling(struct chat_message msg){
    message_history[history_count++ % MAX_HISTORY] = msg;

    pthread_mutex_lock(&buffer.lock);
    buffer.messages[buffer.head] = msg;
    buffer.head = (buffer.head + 1) % MAX_PENDING;
    pthread_mutex_unlock(&buffer.lock);
}

void* meiling_thread(void* args){
    struct message_buffer* buffer = (struct message_buffer*)args;
    
    while (server_runing) {
        pthread_mutex_lock(&buffer->lock);
        if (buffer->tail != buffer->head) {
            struct chat_message msg = buffer->messages[buffer->tail];
            buffer->tail = (buffer->tail + 1) % MAX_PENDING;

            pthread_mutex_unlock(&buffer->lock);
            
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].is_active) {
                    mq_send(clients[i].queue_fd, (char*)&msg, sizeof(msg), 0);
                }
            }
        } else {
            pthread_mutex_unlock(&buffer->lock);
            usleep(5000);
        }
    }
    return(NULL);
}

void register_client(pid_t pid, char *name){
    char queue_name[64];
    snprintf(queue_name, sizeof(queue_name), "/client_%d", pid);

    mqd_t fd = mq_open(queue_name, O_RDWR, 0666, &attr);

    if(client_count < MAX_CLIENTS){
        
        int i = 0;
        while(clients[i].is_active) i++;

        clients[i].pid = pid;
        strncpy(clients[i].queue_name, queue_name, 64);
        clients[i].queue_fd = fd;
        strncpy(clients[i].name, name, 32);
        clients[i].is_active = 1;
        client_count++;

        struct chat_message response = {
            .mtype = MSG_SYSTEM
        };
        strncpy(response.text, queue_name, sizeof(response.text));

        mq_send(fd, (char*)&response, sizeof(response), 0);

        struct chat_message msg = {
            .mtype = MSG_SYSTEM,
            .client_pid = pid
        };
        strncpy(msg.client_name, name, 32);

        for (int i = 0; i < history_count; i++) {
            mq_send(fd, (char*)&message_history[i], sizeof(message_history[i]), 0);
        }

        message_history[history_count++ % MAX_HISTORY] = msg;

        pthread_mutex_lock(&buffer.lock);
        buffer.messages[buffer.head] = msg;
        buffer.head = (buffer.head + 1) % MAX_PENDING;
        pthread_mutex_unlock(&buffer.lock);

    }
    
    else{
        struct chat_message response = {
            .mtype = MSG_SYSTEM,
            .text = "error"
        };
        mq_send(fd, (char*)&response, sizeof(response), 0);
    }
}

void deleted_client(pid_t pid){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].is_active && clients[i].pid == pid){

            struct chat_message msg = {
                .mtype = MSG_SYSTEM
            };
            strncpy(msg.client_name, clients[i].name, 32);
            strncpy(msg.text, "del", 5);

            message_history[history_count++ % MAX_HISTORY] = msg;

            pthread_mutex_lock(&buffer.lock);
            buffer.messages[buffer.head] = msg;
            buffer.head = (buffer.head + 1) % MAX_PENDING;
            pthread_mutex_unlock(&buffer.lock);

            mq_close(clients[i].queue_fd);
            mq_unlink(clients[i].queue_name);

            clients[i].is_active = 0;

            clients[i].pid = 0;
            clients[i].queue_name[0] = '\0';
            clients[i].name[0] = '\0';

            client_count--;

            break;
        }
    }
}