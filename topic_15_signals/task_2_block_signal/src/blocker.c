#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(void){
    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
        perror("sigprocmask failed");
        return 1;
    }

    printf("My pid: %d\n", getpid());

    while(1){
        sleep(2);
    }

    return 0;
}