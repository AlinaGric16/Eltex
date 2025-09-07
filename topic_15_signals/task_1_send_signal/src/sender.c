#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>

int main(void){
    pid_t pid;
    printf("Input pid\n");
    scanf("%d", &pid);

    if (kill(pid, SIGUSR1) == -1) {
        perror("kill failed");
        return 1;
    }

    return 0;
}