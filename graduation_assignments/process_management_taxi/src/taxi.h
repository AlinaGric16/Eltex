#ifndef __TAXI_H__
#define __TAXI_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <poll.h>

#define MAX_MSG 32
#define MAX_DRIVERS 10

#define AVAILABLE 0
#define BUSY 1

#define MSG_SEND_TASK 0
#define MSG_GET_STATUS 1

// Структуры
struct driver{
    pid_t pid;
    int cli_to_driver[2];
    int driver_to_cli[2];
};

struct command{
    int type;
    int task_timer;
};

struct response{
    int type;
    int status;
    int task_timer;
};

// Глобальные переменные
extern struct driver drivers[MAX_DRIVERS];
extern volatile int is_running;

// Сигналы
void signal_handler();
void driver_signal_handler();

// CLI функции
int handle_user_input();
int handle_driver_message(int index);
int create_driver();
int send_task(pid_t pid, int timer);
int get_status(pid_t pid);
int get_drivers();
int find_driver_index(pid_t pid);

// Driver функция
int driver_process(int index);

#endif