#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAX_SIZE 8192

int main(void){
    const char name[MAX_SIZE] = "/chat";
    char msgr[MAX_SIZE];
    char msg[MAX_SIZE] = "Hello!";

    mqd_t fd = mq_open(name, O_RDWR);
    if(fd == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    if(mq_receive(fd, msgr, MAX_SIZE, NULL) == -1){
        perror("faile mq_receive");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", msgr);


    if(mq_send(fd, msg, sizeof(msg), 1) == -1){
        perror("failed mq_send");
        exit(EXIT_FAILURE);
    }

    return 0;
}