#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 20

struct message {
 long type;
 char text[MAX_SIZE];
 } msg;

int main(void){

    int msqid = msgget(1, (IPC_CREAT | 0666));
    if(msqid == -1){
        perror("Failed msgget");
        exit(EXIT_FAILURE);
    }

    msg.type = 1;
    strcpy(msg.text, "Hi!");
    if(msgsnd(msqid, (void *) &msg, sizeof(msg.text), 0) == -1){
        perror("Failed msgsnd");
        exit(EXIT_FAILURE);
    }

    if(msgrcv(msqid, (void *) &msg, MAX_SIZE, 2, 0) == -1){
        perror("Failed msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", msg.text);

    struct msqid_ds buf;
    if(msgctl(msqid, IPC_RMID, &buf) == -1){
        perror("Failed msgrcv");
        exit(EXIT_FAILURE);
    }

    return 0;
}