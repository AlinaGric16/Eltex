#include "client.h"

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

void cleanup_windows() {
    delwin(msg_win);
    delwin(clients_win);
    delwin(input_win);
}

void add_message(const char* text) {
    
    if (msg_line >= LINES - 4) {

        msg_line = 1;
    }
    
    mvwprintw(msg_win, msg_line, 1, "%-*s", getmaxx(msg_win)-2, "");
    mvwprintw(msg_win, msg_line++, 1, "%s", text);
    wrefresh(msg_win);
    
}

void refresh_client_win() {

    int line = 0;
    for(int i = 0; i < MAX_CLIENTS; i++){
        mvwprintw(clients_win, i+1, 1, "%-*s", getmaxx(clients_win)-2, "");
        if(clients[i] != NULL && clients[i][0] != '\0'){
            mvwprintw(clients_win, line+1, 1, "* %s ", clients[i]);
            line++;
        }
    }
    wrefresh(clients_win);
    
}

void del_client(char *name){
    int i = 0;
    while(i < MAX_CLIENTS && strncmp(clients[i], name, 32) != 0){
        i++;
    }
    clients[i][0] = '\0';
}

void* message_receiver(void* args){
    mqd_t *fd_client = (mqd_t*)args;
    struct chat_message msg;
   
    
    while (1) {
        if (mq_receive(*fd_client, (char*)&msg, sizeof(msg), NULL) == -1) {
            perror("mq_receive failed");
            break;
        }
        
        if (msg.mtype == MSG_SYSTEM) {

            char sys_msg[300];
            if(strncmp(msg.text, "del", 5) == 0){
                snprintf(sys_msg, sizeof(sys_msg), "SYSTEM: user %s left the chat", msg.client_name);
                client_line--;
                del_client(msg.client_name);
            }
            else{
                snprintf(sys_msg, sizeof(sys_msg), "SYSTEM: user %s joined the chat", msg.client_name);
                int i;
                for(i = 0; i < MAX_CLIENTS && clients[i] != NULL && clients[i][0] != '\0'; i++);
                strncpy(clients[i], msg.client_name, 32);
                client_line++;
                
            }
            add_message(sys_msg);
            refresh_client_win();
        }
        else if (msg.mtype == MSG_TEXT) {
            char chat_msg[300];
            snprintf(chat_msg, sizeof(chat_msg), "%s: %s", msg.client_name, msg.text);
            add_message(chat_msg);
        }
    }
    return NULL;
}