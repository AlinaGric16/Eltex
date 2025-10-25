#include "taxi.h"

int driver_process(int index){
    struct command cli_msg;
    struct response driver_msg;
    //pid_t pid = getpid();
    int status = AVAILABLE;
    int task_timer = 0;
    time_t task_start_time = 0;

    struct sigaction sa;
    sa.sa_handler = driver_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGTERM, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    close(drivers[index].cli_to_driver[1]);
    close(drivers[index].driver_to_cli[0]);

    while(is_running){
        
        if(status == BUSY){
            int elapsed = time(NULL) - task_start_time;
            
            if (elapsed >= task_timer) {
                status = AVAILABLE;
                task_timer = 0;
            }
            driver_msg.task_timer = (status == BUSY) ? (task_timer - (time(NULL) - task_start_time)) : 0;
        }

        struct pollfd fds[1];
        fds[0].fd = drivers[index].cli_to_driver[0];
        fds[0].events = POLLIN;
        
        int ready = poll(fds, 1, 50);
        
        if (ready > 0 && (fds[0].revents & POLLIN)) {
            if (read(drivers[index].cli_to_driver[0], &cli_msg, sizeof(cli_msg)) != sizeof(cli_msg)){
                perror("failed read in driver");
                break;
            }
            else{
                switch (cli_msg.type) {
                case MSG_SEND_TASK:
                    if (status == BUSY) {
                        driver_msg.type = MSG_SEND_TASK;
                        driver_msg.status = status;
                        
                        if(write(drivers[index].driver_to_cli[1], &driver_msg, sizeof(driver_msg)) == -1){
                            perror("failed write");
                            return 1;
                        }
                    } else {
                        task_timer = cli_msg.task_timer;
                        task_start_time = time(NULL);
                        
                        driver_msg.type = MSG_SEND_TASK;
                        driver_msg.status = status;
                        driver_msg.task_timer = task_timer;

                        status = BUSY;
                        if(write(drivers[index].driver_to_cli[1], &driver_msg, sizeof(driver_msg)) == -1){
                            perror("failed write");
                            return 1;
                        }
                    }
                    break;
                case MSG_GET_STATUS:
                    driver_msg.type = MSG_GET_STATUS;
                    driver_msg.status = status;
                    if(write(drivers[index].driver_to_cli[1], &driver_msg, sizeof(driver_msg)) == -1){
                        perror("failed write");
                        return 1;
                    }
                    break;
                }
            }
        }
        else if (ready == -1 && errno != EINTR) {
            perror("poll failed");
            break;
        }
    }

    close(drivers[index].cli_to_driver[0]);
    close(drivers[index].driver_to_cli[1]);

    return 0;
}

void driver_signal_handler() {
    is_running = 0;
}