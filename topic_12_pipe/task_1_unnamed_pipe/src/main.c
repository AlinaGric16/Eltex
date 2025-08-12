#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(void){
    int status;
    int pipefd[2];
    if(pipe(pipefd) < 0){
        perror("failed pipe");
        exit(EXIT_FAILURE);
    }
    pid_t child_pid = fork();
    if(child_pid < 0){
        perror("failed fork");
        exit(EXIT_FAILURE);
    }
    if(child_pid == 0){
        char buf_r[5];

        close(pipefd[1]);
        
        if(read(pipefd[0], buf_r, 5) < 0){
            perror("failed read");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        printf("%s\n", buf_r);
        exit(EXIT_SUCCESS);
    } else{
        char *buf_w = "Hi!";

        close(pipefd[0]);

        if(write(pipefd[1], buf_w, strlen(buf_w) + 1) < 0){
            perror("failed write");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
        wait(&status);
    }
    return status;
}