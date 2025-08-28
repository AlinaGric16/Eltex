#include "client.h"

struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(struct chat_message)
};

WINDOW *msg_win, *clients_win, *input_win;

char clients[MAX_CLIENTS][32] = {0};
int msg_line = 1;
int client_line = 0;

int main(void){
    initscr();
    cbreak();
    noecho();
    
    printw("Enter your name: ");
    refresh();
    
    char name[32];
    echo();
    getnstr(name, sizeof(name) - 1);
    noecho();
    
    struct chat_message msg;
    pid_t pid = getpid();
    char queue_name[64];
    snprintf(queue_name, sizeof(queue_name), "/client_%d", pid);

    mqd_t fd = mq_open("/chat_message", O_RDWR, 0666, &attr);
    if(fd == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    mqd_t fd_client = mq_open(queue_name, O_CREAT | O_RDWR, 0666, &attr);
    if(fd_client == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    struct chat_message response = {
        .mtype = MSG_REGISTER,
        .client_pid = pid
    };
    strncpy(response.client_name, name, sizeof(response.client_name));

    mq_send(fd, (char*)&response, sizeof(response), 0);

    mq_receive(fd_client, (char*)&msg, sizeof(msg), NULL);

    if(strncmp(msg.text, queue_name, 32) != 0){
        endwin();
        printf("The server is full. Try again later.\n");

        mq_close(fd_client);
        mq_unlink(queue_name);

        return 0;
    }

    keypad(stdscr, TRUE);
    curs_set(0);
    
    setup_windows();
    
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, message_receiver, (void*)&fd_client);
    
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
                struct chat_message out_msg = {
                    .mtype = MSG_TEXT,
                    .client_pid = pid
                };
                strncpy(out_msg.text, input_buffer, sizeof(out_msg.text));
                strncpy(out_msg.client_name, name, sizeof(out_msg.client_name));
                
                mq_send(fd, (char*)&out_msg, sizeof(out_msg), 0);
                
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

    pthread_cancel(receiver_thread);
    pthread_join(receiver_thread, NULL);
    
    cleanup_windows();
    endwin();

    struct chat_message deletion = {
        .mtype = MSG_LEAVE,
        .client_pid = pid
    };
    mq_send(fd, (char*)&deletion, sizeof(deletion), 0);
    
    mq_close(fd_client);
    mq_unlink(queue_name);
    mq_close(fd);
    
    printf("Disconnected from server\n");

    return 0;
}
