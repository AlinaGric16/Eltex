#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    pid_t child_pid = fork();

    if(child_pid == 0){
        printf("Child process:\n");
        printf("  PID = %d\n", getpid());
        printf("  PPID = %d\n", getppid());
        exit(22);
    } else{
        int status;
        wait(&status);

        printf("Parent process:\n");
        printf("  PID = %d\n", getpid());
        printf("  PPID = %d\n", getppid());
        printf("status = %d\n", WEXITSTATUS(status));
    }
}