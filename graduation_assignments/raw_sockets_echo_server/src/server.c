#include "server.h"

volatile int server_running = 1;

int main(void){
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    struct Client{
        char client_ip[INET_ADDRSTRLEN];
        unsigned short client_port;
        int counter;
    };
    struct Client clients[MAX_CLI] = {0};

    struct sockaddr_in serv, cli;
    socklen_t struct_cli_len = sizeof(cli);
    char buf[MAX_MSG];

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);

    if(bind(fd, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started\nTo stop Ctrl+C\n");

    while (server_running)
    {
        memset(buf, 0, MAX_MSG);

        if(recvfrom(fd, &buf, MAX_MSG-1, 0, (struct sockaddr*)&cli, &struct_cli_len) == -1){
            if (errno != EINTR) {
                perror("failed recvfrom");
                close(fd);
                exit(EXIT_FAILURE);
            }
            continue;
        }

        int tmp = -1;
        int empty = -1;
        unsigned short tmp_port = ntohs(cli.sin_port);

        for(int i = 0; i < MAX_CLI && tmp == -1; i++){
            if(clients[i].client_port == tmp_port){
                clients[i].counter++;
                tmp = i;
            }
            if(empty == -1 && clients[i].client_port == 0)
                empty = i;
        }
        
        if(tmp == -1){
            tmp = empty;
            if (tmp == -1) {
                strncpy(buf, "Server is busy", 15);
                if(sendto(fd, &buf, strlen(buf)+1, 0, (struct sockaddr*)&cli, struct_cli_len) == -1){
                    perror("failed sendto");
                }
                continue;
            }
            inet_ntop(AF_INET, &(cli.sin_addr), clients[tmp].client_ip, INET_ADDRSTRLEN);
            clients[tmp].client_port = tmp_port;
            clients[tmp].counter = 1;
        }

        if(!strncmp(buf, "exit", 4)){
            clients[tmp] = (struct Client){0};
            continue;
        }

        char tmp_buf[MAX_MSG - MAX_NUM];
        strncpy(tmp_buf, buf, MAX_MSG - MAX_NUM);
        tmp_buf[sizeof(tmp_buf) - 1] = '\0';

        snprintf(buf, MAX_MSG, "%s %d", tmp_buf, clients[tmp].counter);

        if(sendto(fd, &buf, strlen(buf)+1, 0, (struct sockaddr*)&cli, struct_cli_len) == -1){
            perror("failed sendto");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    printf("\nServer stopped\n");
    close(fd);
    return 0;
}

void signal_handler() {
    server_running = 0;
}
