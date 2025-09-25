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
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#define MAX_MSG 2048
#define PORT_SOURCE 8888
#define PORT_DEST 7777

unsigned short calculate_ip_checksum(struct iphdr *ip);

#endif