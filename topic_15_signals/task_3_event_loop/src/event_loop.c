#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void){
    sigset_t wait_set;
    sigemptyset(&wait_set);
    sigaddset(&wait_set, SIGUSR1);
    int caught_signal;

    if (sigprocmask(SIG_BLOCK, &wait_set, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    printf("My pid: %d\n", getpid());

    while(1){
        printf("Waiting for the signal\n");
        
        if (sigwait(&wait_set, &caught_signal) == -1) {
            perror("sigwait failed");
            return 1;
        }
        
        printf("Received a signal: %d (%s)\n", caught_signal, caught_signal == SIGUSR1 ? "SIGUSR1" : "UNKNOWN");
    }

    return 0;
}
