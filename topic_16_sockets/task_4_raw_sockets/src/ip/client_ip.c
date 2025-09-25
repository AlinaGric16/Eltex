#include "client.h"

int main(void){
    char buf[] = "Hello!";

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    int flag = 1;
    if(setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) == -1){
        perror("failed setsockopt");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int size_packet = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(buf) + 1;
    char packet[size_packet];
    struct iphdr *ip = (struct iphdr*)packet;
    struct udphdr *udp = (struct udphdr*)(packet + sizeof(struct iphdr));
    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(size_packet);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    inet_pton(AF_INET, "127.0.0.1", &ip->saddr);
    inet_pton(AF_INET, SERVER_IP, &ip->daddr);

    udp->source = htons(PORT_SOURCE);
    udp->dest = htons(PORT_DEST);
    udp->len =  htons(sizeof(struct udphdr) + strlen(buf) + 1);;
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
