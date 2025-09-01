#include "client.h"

int users = 0;

void setup_windows() {
    
    int height = LINES - 3;
    int msg_width = COLS * 2 / 3;
    int input_height = 3;
    
    msg_win = newwin(height, msg_width, 0, 0);
    clients_win = newwin(height, COLS - msg_width, 0, msg_width);
    input_win = newwin(input_height, COLS, height, 0);
    
    box(msg_win, 0, 0);
    box(clients_win, 0, 0);
    box(input_win, 0, 0);
    
    mvwprintw(msg_win, 0, 2, " Messages ");
    mvwprintw(clients_win, 0, 2, " Users ");
    mvwprintw(input_win, 0, 2, " Input ");
    
    wrefresh(msg_win);
    wrefresh(clients_win);
    wrefresh(input_win);
    refresh();
}

void* message_receiver(void* args){
    struct info *inf = (struct info*)args;
    struct chat_room *chat = (struct chat_room*)shmat(inf->shm_id, NULL, 0);
   
    
    struct sembuf wait_sem_nowait = {0, -1, IPC_NOWAIT};
    
    while (!inf->should_exit) {

        int result = semop(inf->id_my_sem, &wait_sem_nowait, 1);
        
        if (result == -1) {
            if (errno == EAGAIN) {

                usleep(100000);
                continue;
            } else {
                perror("semop failed");
                break;
            }
        }
        
        if (semop(inf->id_mem_access, &wait_sem, 1) == -1) {
            perror("semop decrement failed");
            exit(EXIT_FAILURE);
        }
        
        if (chat->user_count != users) {
            refresh_client_win(chat);
        }

        add_message(chat);

        if (semop(inf->id_mem_access, &post_sem, 1) == -1) {
            perror("semop decrement failed");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

void refresh_client_win(struct chat_room *chat) {

    int line = 0;
    for(int i = 0; i < MAX_COUNT_CLIENTS; i++){
        mvwprintw(clients_win, i+1, 1, "%-*s", getmaxx(clients_win)-2, "");
        if(chat->users[i] != NULL && chat->users[i][0] != '\0'){
            mvwprintw(clients_win, line+1, 1, "* %s ", chat->users[i]);
            line++;
        }
    }
    wrefresh(clients_win);
}

void add_message(struct chat_room *chat) {
    int height = getmaxy(msg_win)-2;
    int j = 0;
    if(chat->message_count > height){
        j = chat->message_count - height;
    }
    
    int line = 0;
    for(int i = 0; i < height; i++){
        mvwprintw(msg_win, i+1, 1, "%-*s", getmaxx(msg_win)-2, "");
        if(chat->history[j] != NULL){
            mvwprintw(msg_win, line+1, 1, "%s ", chat->history[j]);
            line++;
            j++;
        }
    }
    wrefresh(msg_win);
    
}

void del_client(struct chat_room *chat, char *client_name){
    char msg[MAX_LENGHT_MSG];
    snprintf(msg, MAX_LENGHT_MSG, "SYSTEM: user %s left the chat", client_name);
    int flag = 0;

    for(int i = 0; i < MAX_COUNT_CLIENTS; i++){
        if(strncmp(chat->users[i], client_name, MAX_CLIENT_NAME) == 0){
            flag = 1;
        }
        if(flag){
            if(i == MAX_COUNT_CLIENTS-1 || chat->users[i+1] == NULL || chat->users[i+1][0] == '\0'){
                chat->users[i][0] = '\0';
                chat->client_semaphores[i] = 0;
            }
            else{
                strncpy(chat->users[i], chat->users[i+1], MAX_CLIENT_NAME);
                chat->client_semaphores[i] = chat->client_semaphores[i+1];
            }
        }
    }

    strncpy(chat->history[chat->message_count], msg, MAX_LENGHT_MSG);

    chat->message_count++;
    chat->user_count--;
}

void cleanup_windows() {
    delwin(msg_win);
    delwin(clients_win);
    delwin(input_win);
}