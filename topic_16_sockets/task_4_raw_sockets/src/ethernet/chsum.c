#include "client.h"

unsigned short calculate_ip_checksum(struct iphdr *ip) {
    unsigned int sum = 0;
    unsigned short *ptr = (unsigned short*)ip;
    int length = ip->ihl * 2;
    
    for (int i = 0; i < length; i++) {
        sum += ptr[i];
    }
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return (unsigned short)~sum;
}