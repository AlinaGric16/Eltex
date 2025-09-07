#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sigusr1_handler()
{
    printf("The SIGUSR1 signal was received\n");
}

int main(void){
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if(sigaction(SIGUSR1, &sa, NULL) == -1){
        perror("sigaction failed");
        return 1;
    }

    printf("My pid: %d\n", getpid());

    while(1) {
        sleep(1);
    }

    return 0;
}
