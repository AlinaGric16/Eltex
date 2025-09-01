#include "client.h"

struct sembuf post_sem = {0, 1, 0};
struct sembuf wait_sem = {0, -1, 0};

int main(void){
    initscr();
    cbreak();
    noecho();
    
    printw("Enter your name: ");
    refresh();
    
    char client_name[MAX_CLIENT_NAME];
    echo();
    getnstr(client_name, sizeof(client_name) - 1);
    client_name[MAX_CLIENT_NAME-1] = '\0';
    noecho();

    char *name = "./server_chat";

    key_t chat_key = ftok(name, 1);
    if(chat_key == -1){
        perror("failed ftok for shared memory");
        exit(EXIT_FAILURE);
    }

    int chat_id = shmget(chat_key, sizeof(struct chat_room), 0);
    if(chat_id == -1){
        perror("failed shmget");
        exit(EXIT_FAILURE);
    }

    struct chat_room *chat = (struct chat_room*)shmat(chat_id, NULL, 0);
    if (chat == (void*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    key_t memory_access_key = ftok(name, 2);
    key_t new_data_key = ftok(name, 3);
    if(memory_access_key == -1 || new_data_key == -1){
        shmdt(chat);
        perror("failed ftok for semaphore");
        exit(EXIT_FAILURE);
    }

    int memory_access = semget(memory_access_key, 1, IPC_CREAT | 0666);
    int new_data = semget(new_data_key, 1, IPC_CREAT | 0666);
    if(memory_access == -1 || memory_access == -1){
        shmdt(chat);
        perror("failed semget");
        exit(EXIT_FAILURE);
    }

    if (semop(memory_access, &wait_sem, 1) == -1) {
        shmdt(chat);
        perror("semop decrement failed");
        exit(EXIT_FAILURE);
    }

    if(chat->user_count == MAX_COUNT_CLIENTS){
        endwin();
        printf("The server is full. Try again later.\n");
        semop(memory_access, &post_sem, 1);
        shmdt(chat);
        exit(EXIT_SUCCESS);
    }

    int my_sem = semget(MAX_COUNT_CLIENTS + chat->user_count, 1, IPC_CREAT | 0666);
    if(my_sem == -1){
        semop(memory_access, &post_sem, 1);
        shmdt(chat);
        perror("failed semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(my_sem, 0, SETVAL, 0) == -1) {
        semop(memory_access, &post_sem, 1);
        semctl(my_sem, 0, IPC_RMID);
        shmdt(chat);
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }

    char msg[MAX_LENGHT_MSG];
    snprintf(msg, MAX_LENGHT_MSG, "SYSTEM: user %s joined the chat", client_name);

    strncpy(chat->users[chat->user_count], client_name, MAX_CLIENT_NAME);
    strncpy(chat->history[chat->message_count], msg, MAX_LENGHT_MSG);
    chat->client_semaphores[chat->user_count] = my_sem;
    chat->message_count++;
    chat->user_count++;

    if (semop(memory_access, &post_sem, 1) == -1) {
        semctl(my_sem, 0, IPC_RMID);
        shmdt(chat);
        perror("semop increment failed");
        exit(EXIT_FAILURE);
    }

    if (semop(new_data, &post_sem, 1) == -1) {
        semctl(my_sem, 0, IPC_RMID);
        shmdt(chat);
        perror("semop decrement failed");
        exit(EXIT_FAILURE);
    }

    keypad(stdscr, TRUE);
    curs_set(0);
    
    setup_windows();

    struct info inf =
    {
        .id_mem_access = memory_access,
        .id_my_sem = my_sem,
        .shm_id = chat_id,
        .should_exit = 0
    };

    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, message_receiver, (void*)&inf);
    
    int ch;
    int imput_width = getmaxx(input_win) - 5;
    char input_buffer[imput_width + 1];
    int input_pos = 0;

    mvwprintw(input_win, 1, 1, "%-*s", getmaxx(input_win)-2, "");
    mvwprintw(input_win, 1, 1, "> ");
    mvwprintw(input_win, 2, getmaxx(input_win)-12, " Exit: esc ");
    wrefresh(input_win);

    while((ch = wgetch(input_win)) != 27) {
        if (ch == '\n') {
            if (input_pos > 0) {
                if (semop(memory_access, &wait_sem, 1) == -1) {
                    clean(receiver_thread, my_sem, chat);
                    perror("semop decrement failed");
                    exit(EXIT_FAILURE);
                }

                snprintf(msg, MAX_LENGHT_MSG, "%s: %s", client_name, input_buffer);

                strncpy(chat->history[chat->message_count], msg, MAX_LENGHT_MSG);
                chat->message_count++;

                if (semop(memory_access, &post_sem, 1) == -1) {
                    clean(receiver_thread, my_sem, chat);
                    perror("semop increment failed");
                    exit(EXIT_FAILURE);
                }

                if (semop(new_data, &post_sem, 1) == -1) {
                    clean(receiver_thread, my_sem, chat);
                    perror("semop decrement failed");
                    exit(EXIT_FAILURE);
                }
                
                input_buffer[0] = '\0';
                input_pos = 0;
                
                mvwprintw(input_win, 1, 1, "%-*s", getmaxx(input_win)-2, "");
                mvwprintw(input_win, 1, 1, "> ");
                wrefresh(input_win);
            }
        }
        else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (input_pos > 0) {
                input_buffer[--input_pos] = '\0';
            }
        }
        else if (input_pos < imput_width && ch >= 32 && ch <= 126) {
            input_buffer[input_pos++] = ch;
            input_buffer[input_pos] = '\0';
        }
        
        mvwprintw(input_win, 1, 3, "%-*s", imput_width, input_buffer);
        
        if (input_pos < imput_width) {
            mvwprintw(input_win, 1, 3 + input_pos, "_");
        }
        
        wrefresh(input_win);
        napms(50);
    }


    inf.should_exit = 1;
    pthread_cancel(receiver_thread);
    pthread_join(receiver_thread, NULL);

    cleanup_windows();
    endwin();

    if (semop(memory_access, &wait_sem, 1) == -1) {
        clean(receiver_thread, my_sem, chat);
        perror("semop decrement failed");
        exit(EXIT_FAILURE);
    }

    del_client(chat, client_name);

    if (semop(memory_access, &post_sem, 1) == -1) {
        clean(receiver_thread, my_sem, chat);
        perror("semop increment failed");
        exit(EXIT_FAILURE);
    }

    if (semop(new_data, &post_sem, 1) == -1) {
        clean(receiver_thread, my_sem, chat);
        perror("semop decrement failed");
        exit(EXIT_FAILURE);
    }

    semctl(my_sem, 0, IPC_RMID);
    shmdt(chat);

    printf("Disconnected from server\n");

    return 0;
}

void clean(pthread_t receiver_thread, int my_sem, struct chat_room *chat){
    pthread_cancel(receiver_thread);
    pthread_join(receiver_thread, NULL);
    cleanup_windows();
    endwin();
    semctl(my_sem, 0, IPC_RMID);
    shmdt(chat);
}