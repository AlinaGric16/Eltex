#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

#define MAX_MSG 2048
#define PORT_SOURCE 8888
#define PORT_DEST 7777
#define SERVER_IP "127.0.0.1"

#endif