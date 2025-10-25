#include "taxi.h"

void signal_handler() {
    is_running = 0;
}

int handle_user_input(){
    char input[MAX_MSG] = {0};

    fflush(stdout);

    if(!fgets(input, MAX_MSG + 1, stdin)){
        perror("input failed");
        return -1;
    }

    if (strlen(input) >= MAX_MSG && input[MAX_MSG - 1] != '\n') {
        printf("Message too long, maximum %d characters\n", MAX_MSG - 1);
        while(getchar() != '\n');
        return -1;
    }
    input[strcspn(input, "\n")] = '\0';

    char command[20], arg1[20], arg2[20];
    int parsed = sscanf(input, "%19s %19s %19s", command, arg1, arg2);

    if (parsed < 1) {
        printf("Invalid command\n");
        return -1;
    }

    if(!strncmp(command, "create_driver", 20)) create_driver();
    else if(!strncmp(command, "send_task", 20)){
        if (parsed < 3) {
            printf("Usage: send_task <pid> <task_timer>\n");
            return -1;
        }
        pid_t pid = atoi(arg1);
        int timer = atoi(arg2);
        send_task(pid, timer);
    }
    else if(!strncmp(command, "get_status", 20)){
        if (parsed < 2) {
            printf("Usage: get_status <pid>\n");
            return -1;
        }
        pid_t pid = atoi(arg1);
        get_status(pid);
    }
    else if(!strncmp(command, "get_drivers", 20)){
        get_drivers();
    }
    else if(!strncmp(command, "exit", 4)) is_running = 0;
    else {
        printf("Unknown command: %s\n", command);
    }
    
    return 0;
}

int create_driver(){
    int index = -1;
    for(int i = 0; i < MAX_DRIVERS; i++){
        if(drivers[i].pid == 0){
            index = i;
            break;
        }
    }

    if(index == -1){
        printf("Maximum drivers reached\n");
        return -1;
    }

    pid_t child_pid = fork();

    if(child_pid < 0){
        perror("fork failed");
        return -1;
    }
    if(child_pid == 0){
        driver_process(index);
        exit(0);
    } else{
        close(drivers[index].cli_to_driver[0]);
        close(drivers[index].driver_to_cli[1]);

        drivers[index].pid = child_pid;

        printf("Created driver with PID: %d\n", child_pid);

        return 0;
    }
}

int send_task(pid_t pid, int timer){
    if (pid <= 0) {
        printf("Invalid PID\n");
        return -1;
    }
    if (timer <= 0) {
        printf("Timer must be positive\n");
        return -1;
    }

    int index = find_driver_index(pid);
    
    if (index == -1) {
        printf("Driver with PID %d not found\n", pid);
        return -1;
    }

    struct command task = {
        .type = MSG_SEND_TASK,
        .task_timer = timer
    };

    if(write(drivers[index].cli_to_driver[1], &task, sizeof(task)) == -1){
        perror("Failed to send task to driver");
        return -1;
    }

    return 0;
}

int get_status(pid_t pid){
    int index = find_driver_index(pid);
    
    if (index == -1) {
        printf("Driver with PID %d not found\n", pid);
        return -1;
    }

    struct command get = {
        .type = MSG_GET_STATUS
    };

    if(write(drivers[index].cli_to_driver[1], &get, sizeof(get)) == -1){
        perror("Failed to send task to driver");
        return -1;
    }

    return 0;
}

int get_drivers(){
    printf("Active drivers:\n");
    int count = 0, error = 0;

    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].pid != 0) {
            if (get_status(drivers[i].pid) != 0) {
                error = 1;
            }
            count++;
        }
    }

    if (count == 0)
        printf("  No active drivers\n");

    return error;
}

int find_driver_index(pid_t pid) {
    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

int handle_driver_message(int index){
    struct response driver_msg;

    if(read(drivers[index].driver_to_cli[0], &driver_msg, sizeof(driver_msg)) == -1){
        perror("failed read driver message");
        return -1;
    }
    else{
        switch (driver_msg.type)
        {
        case MSG_SEND_TASK:
            if(driver_msg.status == BUSY){
                printf("Busy %d\n", driver_msg.task_timer);
            }
            else{
                printf("Driver %d started task %d\n", drivers[index].pid, driver_msg.task_timer);
            }
            break;
        
        case MSG_GET_STATUS:
            if(driver_msg.status == BUSY){
                printf("Driver %d: Busy %d\n", drivers[index].pid, driver_msg.task_timer);
            }
            else{
                printf("Driver %d: Available\n", drivers[index].pid);
            }
            break;
            
        }
    }
    return 0;
}