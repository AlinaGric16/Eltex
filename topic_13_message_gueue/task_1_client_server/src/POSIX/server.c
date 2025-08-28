#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SIZE 8192

int main(void){
    const char name[MAX_SIZE] = "/chat";
    char msg[MAX_SIZE] = "Hi!";
    char msgr[MAX_SIZE];

    mqd_t fd = mq_open(name, O_CREAT | O_RDWR, 0666, NULL);
    if(fd == -1){
        perror("failed mq_open");
        exit(EXIT_FAILURE);
    }

    if(mq_send(fd, msg, sizeof(msg), 1) == -1){
        perror("failed mq_send");
        exit(EXIT_FAILURE);
    }
    
    sleep(5);

    if(mq_receive(fd, msgr, MAX_SIZE, NULL) == -1){
        perror("faile mq_receive");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", msgr);

    mq_close(fd);
    mq_unlink(name);

    return 0;
}