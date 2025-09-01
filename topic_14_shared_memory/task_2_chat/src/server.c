#include "server.h"
int main(void){
    char *name = "./server_chat";

    int chat_fd = open(name, O_CREAT | O_RDWR, 0666);
    if(chat_fd == -1){
        perror("failed open");
        exit(EXIT_FAILURE);
    }
    close(chat_fd);

    key_t chat_key = ftok(name, 1);
    if(chat_key == -1){
        unlink(name);
        perror("failed ftok for shared memory");
        exit(EXIT_FAILURE);
    }

    int chat_id = shmget(chat_key, sizeof(struct chat_room), IPC_CREAT | 0666);
    if(chat_id == -1){
        unlink(name);
        perror("failed shmget");
        exit(EXIT_FAILURE);
    }

    struct chat_room *chat = (struct chat_room*)shmat(chat_id, NULL, 0);
    if (chat == (void*)-1) {
        shmctl(chat_id, IPC_RMID, NULL);
        unlink(name);
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    chat->user_count = 0;
    chat->message_count = 0;

    key_t memory_access_key = ftok(name, 2);
    key_t new_data_key = ftok(name, 3);
    if(memory_access_key == -1 || new_data_key == -1){
        shmdt(chat);
        shmctl(chat_id, IPC_RMID, NULL);
        unlink(name);
        perror("failed ftok for semaphore");
        exit(EXIT_FAILURE);
    }

    int memory_access = semget(memory_access_key, 1, IPC_CREAT | 0666);
    int new_data = semget(new_data_key, 1, IPC_CREAT | 0666);
    if(memory_access == -1 || new_data == -1){
        shmdt(chat);
        shmctl(chat_id, IPC_RMID, NULL);
        unlink(name);
        perror("failed semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(memory_access, 0, SETVAL, 1) == -1 ||\
        semctl(new_data, 0, SETVAL, 0) == -1) {
        clear(memory_access, new_data, chat, chat_id, name);
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }
    struct sembuf post_sem = {0, 1, 0};
    struct sembuf wait_sem = {0, -1, 0};

    while(1){ 
        if (semop(new_data, &wait_sem, 1) == -1) {
            clear(memory_access, new_data, chat, chat_id, name);
            perror("semop decrement1 failed");
            exit(EXIT_FAILURE);
        }

        if (semop(memory_access, &wait_sem, 1) == -1) {
            clear(memory_access, new_data, chat, chat_id, name);
            perror("semop decrement2 failed");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < chat->user_count; i++){
            if (semop(chat->client_semaphores[i], &post_sem, 1) == -1) {
                clear(memory_access, new_data, chat, chat_id, name);
                perror("semop increment1 failed");
                exit(EXIT_FAILURE);
            }
            printf("Send user %d\n", chat->client_semaphores[i]);
        }

        if (semop(memory_access, &post_sem, 1) == -1) {
            clear(memory_access, new_data, chat, chat_id, name);
            perror("semop increment2 failed");
            exit(EXIT_FAILURE);
        }
    }

    clear(memory_access, new_data, chat, chat_id, name);
    return 0;
}

void clear(int memory_access, int new_data,struct chat_room *chat, int chat_id, char *name){
    semctl(memory_access, 0, IPC_RMID);
    semctl(new_data, 0, IPC_RMID);
    shmdt(chat);
    shmctl(chat_id, IPC_RMID, NULL);
    unlink(name);
}