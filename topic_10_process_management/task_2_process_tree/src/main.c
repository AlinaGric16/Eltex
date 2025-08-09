#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    pid_t child_pid_1 = fork();
    int status;

    if(child_pid_1 < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(child_pid_1 == 0){
        pid_t child_pid_3 = fork();

        if(child_pid_3 < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(child_pid_3 == 0){
            printf("Process 3: pid = %d, ppid = %d\n", getpid(), getppid());
            exit(3);
        } else{
            pid_t child_pid_4 = fork();

            if(child_pid_4 < 0){
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            if(child_pid_4 == 0){
                printf("Process 4: pid = %d, ppid = %d\n", getpid(), getppid());
                exit(4);
            } else{
                printf("Process 1: pid = %d, ppid = %d\n", getpid(), getppid());
                waitpid(child_pid_3, &status, 0);
                printf("status = %d\n", WEXITSTATUS(status));
                waitpid(child_pid_4, &status, 0);
                printf("status = %d\n", WEXITSTATUS(status));
            }
        }
        exit(1);
    } else{
        pid_t child_pid_2 = fork();

        if(child_pid_2 < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(child_pid_2 == 0){
            pid_t child_pid_5 = fork();

            if(child_pid_5 < 0){
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            if(child_pid_5 == 0){
                printf("Process 5: pid = %d, ppid = %d\n", getpid(), getppid());
                exit(5);
            } else{
                printf("Process 2: pid = %d, ppid = %d\n", getpid(), getppid());
                waitpid(child_pid_5, &status, 0);
                printf("status = %d\n", WEXITSTATUS(status));
            }
            exit(2);
        } else{
            printf("Main process: pid = %d, ppid = %d\n", getpid(), getppid());
            waitpid(child_pid_1, &status, 0);
            printf("status = %d\n", WEXITSTATUS(status));
            waitpid(child_pid_2, &status, 0);
            printf("status = %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}