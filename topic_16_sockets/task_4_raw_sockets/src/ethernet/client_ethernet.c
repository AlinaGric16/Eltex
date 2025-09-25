#include "client.h"

int main(int argc, char *argv[]){
    if (argc != 5) {
        printf("Usage: %s <source_ip> <dest_ip> <source_mac> <dest_mac>\n", argv[0]);
        printf("Example: %s 192.168.1.10 192.168.1.20 08:00:27:a6:1a:65 08:00:27:85:51:96\n", argv[0]);
        exit(1);
    }
    
    char *source_ip = argv[1];
    char *dest_ip = argv[2];
    char *source_mac_str = argv[3];
    char *dest_mac_str = argv[4];
    
    unsigned char source_mac[6], dest_mac[6];
    
    if (sscanf(source_mac_str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
               &source_mac[0], &source_mac[1], &source_mac[2],
               &source_mac[3], &source_mac[4], &source_mac[5]) != 6) {
        printf("Invalid source MAC format!\n");
        exit(1);
    }
    
    if (sscanf(dest_mac_str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
               &dest_mac[0], &dest_mac[1], &dest_mac[2],
               &dest_mac[3], &dest_mac[4], &dest_mac[5]) != 6) {
        printf("Invalid destination MAC format!\n");
        exit(1);
    }

    char buf[] = "Hello!";

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd == -1){
        perror("failed socket");
        exit(EXIT_FAILURE);
    }

    int size_packet = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(buf) + 1;
    char packet[size_packet];
    struct ethhdr *mac = (struct ethhdr*)packet;
    struct iphdr *ip = (struct iphdr*)(packet + sizeof(struct ethhdr));
    struct udphdr *udp = (struct udphdr*)(packet + sizeof(struct ethhdr) + sizeof(struct iphdr));
    char *data = packet + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

    memcpy(mac->h_dest, dest_mac, 6);
    memcpy(mac->h_source, source_mac, 6);
    mac->h_proto = htons(ETH_P_IP);

    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(buf) + 1);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    inet_pton(AF_INET, source_ip, &ip->saddr);
    inet_pton(AF_INET, dest_ip, &ip->daddr);

    udp->source = htons(PORT_SOURCE);
    udp->dest = htons(PORT_DEST);
    udp->len =  htons(sizeof(struct udphdr) + strlen(buf) + 1);;
    udp->check = 0;       

    ip->check = calculate_ip_checksum(ip);
    
    strncpy(data, buf, strlen(buf)+1);

    struct sockaddr_ll serv;
    memset(&serv, 0, sizeof(serv));

    serv.sll_family = AF_PACKET;
    serv.sll_ifindex = if_nametoindex("enp0s3");
    serv.sll_halen = ETH_ALEN;
    memcpy(serv.sll_addr, dest_mac, 6);

    if(sendto(fd, packet, size_packet, 0, (struct sockaddr*)&serv, sizeof(serv)) == -1){
        perror("failed sendto");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    while(1){
        char recv_buf[MAX_MSG];

        if((recvfrom(fd, recv_buf, MAX_MSG, 0, NULL, NULL)) == -1){
            perror("failed recvfrom");
            continue;
        }

        struct ethhdr *eth_header = (struct ethhdr*)recv_buf;
        if (eth_header->h_proto != htons(ETH_P_IP)) {
            continue;
        }
       
        struct iphdr *ip_header = (struct iphdr*)(recv_buf + sizeof(struct ethhdr));
        if (ip_header->protocol != IPPROTO_UDP) {
            continue;
        }

        int ip_header_length = ip_header->ihl * 4;

        struct udphdr *udp_header = (struct udphdr*)(recv_buf + sizeof(struct ethhdr) + ip_header_length);
        if (ntohs(udp_header->source) == PORT_DEST && ntohs(udp_header->dest) == PORT_SOURCE) {
            char *response_data = recv_buf + sizeof(struct ethhdr) + ip_header_length + sizeof(struct udphdr);
            printf("%s\n", response_data);
            break;
        }
    }
    close(fd);
    return 0;
}