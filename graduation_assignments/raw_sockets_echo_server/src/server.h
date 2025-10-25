#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define MAX_MSG 1024
#define PORT 7777
#define MAX_CLI 10
#define MAX_NUM 50

void signal_handler();

#endif