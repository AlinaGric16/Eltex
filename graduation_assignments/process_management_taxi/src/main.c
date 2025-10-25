#include "taxi.h"

struct driver drivers[MAX_DRIVERS] = {0};
volatile int is_running = 1;

int main(){
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    int max_fd = STDIN_FILENO;
    for(int i = 0; i < MAX_DRIVERS; i++){
        if(pipe(drivers[i].cli_to_driver) < 0 || pipe(drivers[i].driver_to_cli) < 0){
            perror("failed pipe");
            exit(EXIT_FAILURE);
        }
        FD_SET(drivers[i].driver_to_cli[0], &read_fds);
        if(drivers[i].driver_to_cli[0] > max_fd) {
            max_fd = drivers[i].driver_to_cli[0];
        }
    }

    printf("Taxi manager started ('exit' to quit)\n");
    printf("Available commands: create_driver, send_task <pid> <timer>, get_status <pid>, get_drivers\n");

    while (is_running) {
        fd_set temp_fds = read_fds;
        
        int ready = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if(ready == -1){
            if (errno != EINTR) {
                perror("failed select");
            }
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &temp_fds)) {
            handle_user_input();  
        }
        else{
            for(int i = 0; i < MAX_DRIVERS; i++){
                if(FD_ISSET(drivers[i].driver_to_cli[0], &temp_fds) && is_running){
                    handle_driver_message(i);
                }
            }
        }
    }

    for(int i = 0; i < MAX_DRIVERS; i++){
        if(drivers[i].pid != 0){
            close(drivers[i].cli_to_driver[1]);
            close(drivers[i].driver_to_cli[0]);
        }
    }
    printf("Waiting for drivers to finish...\n");
    sleep(1);
    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].pid != 0) {
            kill(drivers[i].pid, SIGTERM);
            waitpid(drivers[i].pid, NULL, 0);
        }
    }

    printf("\nTaxi manager shat down\n");
    
    return 0;
}
