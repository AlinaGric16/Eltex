#include "client.h"

int main(void){
    char buf[] = "Hello!";

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    int size_packet = sizeof(struct udphdr) + strlen(buf) + 1;
    char packet[size_packet];
    struct udphdr *udp = (struct udphdr*)packet;
    char *data = packet + sizeof(struct udphdr);

    udp->source = htons(PORT_SOURCE);
    udp->dest = htons(PORT_DEST);
    udp->len = htons(size_packet);
    udp->check = 0;       
    
    strncpy(data, buf, strlen(buf)+1);

    struct sockaddr_in serv;

    serv.sin_family = AF_INET;
    if(inet_pton(AF_INET, SERVER_IP, &serv.sin_addr) <= 0){
        perror("failed inet_pton");
        close(fd);
        exit(EXIT_FAILURE);
    }
    serv.sin_port = 0;

    if(sendto(fd, packet, size_packet, 0, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }

    while(1){
        char recv_buf[MAX_MSG];

        if(recvfrom(fd, recv_buf, MAX_MSG, 0, NULL, NULL) == -1){
            perror("failed recvfrom");
            continue;
        }

        struct iphdr *ip_header = (struct iphdr*)recv_buf;
        if (ip_header->protocol != IPPROTO_UDP) {
            continue;
        }

        int ip_header_length = ip_header->ihl * 4;

        struct udphdr *udp_header = (struct udphdr*)(recv_buf + ip_header_length);
        if (ntohs(udp_header->source) == PORT_DEST && ntohs(udp_header->dest) == PORT_SOURCE) {
            char *response_data = recv_buf + ip_header_length + sizeof(struct udphdr);
            printf("%s\n", response_data);
            break;
        }
    }
    close(fd);
    return 0;
}
