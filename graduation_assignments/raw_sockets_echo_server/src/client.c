#include "client.h"

int main(void){
    char input[MAX_MSG];
    int PORT_SOURCE = get_free_port();

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

    struct sockaddr_in serv;

    serv.sin_family = AF_INET;
    if(inet_pton(AF_INET, SERVER_IP, &serv.sin_addr) <= 0){
        perror("failed inet_pton");
        close(fd);
        exit(EXIT_FAILURE);
    }
    serv.sin_port = 0;

    printf("This is an echo server ('exit' to quit)\n");

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if(!fgets(input, MAX_MSG - MAX_NUM + 1, stdin)){
            perror("input failed");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (strlen(input) >= MAX_MSG - MAX_NUM && input[MAX_MSG - MAX_NUM - 1] != '\n') {
            printf("Message too long, maximum %d characters\n", MAX_MSG - MAX_NUM - 1);
            while(getchar() != '\n');
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        int size_packet = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(input) + 1;
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
        udp->len =  htons(sizeof(struct udphdr) + strlen(input) + 1);;
        udp->check = 0;       
        
        strncpy(data, input, strlen(input)+1);

        if(sendto(fd, packet, size_packet, 0, (struct sockaddr*)&serv, sizeof(serv)) == -1){
            perror("failed sendto");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if(!strncmp(input, "exit", 4)) exit(EXIT_SUCCESS);

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
    }

    close(fd);
    return 0;
}

uint16_t get_free_port() {
    srand(time(NULL));
    uint16_t port;
    int attempts = 50;
    
    while (attempts-- > 0) {
        port = 1024 + (rand() % (2024 - 1024));
        
        int test_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (test_fd == -1) continue;

        int flag = 1;
        setsockopt(test_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
        
        struct sockaddr_in test_addr;
        test_addr.sin_family = AF_INET;
        test_addr.sin_addr.s_addr = INADDR_ANY;
        test_addr.sin_port = htons(port);
        
        if (bind(test_fd, (struct sockaddr*)&test_addr, sizeof(test_addr)) == 0) {
            close(test_fd);
            return port;
        }
        
        close(test_fd);
    }

    return 1024 + (rand() % (2024 - 1024));
}
