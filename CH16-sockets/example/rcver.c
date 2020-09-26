#include "proto.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define IPSTRSIZE 40

int main(void)
{
    int sd;
    struct sockaddr_in laddr, raddr;
    uint32_t addr;
    struct msg_st* buf;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];
    int size;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("socket");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RECPORT));
    inet_pton(AF_INET, "0.0.0.0", &addr);
    laddr.sin_addr.s_addr = addr;

    if (bind(sd, (void*)&laddr, sizeof(laddr)) < 0) {
        perror("bind");
        exit(1);
    }

    raddr_len = sizeof(raddr);

    size = sizeof(struct msg_st) + NAMEMAX - 1;
    buf = malloc(size);
    if (buf == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    while (1) {

        memset(buf, '\0', size);

        recvfrom(sd, buf, size, 0, (void*)&raddr, &raddr_len);
        inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ipstr, IPSTRSIZE);
        printf("from  %s:%d\n", ipstr, ntohs(raddr.sin_port));
        printf("NAME = %s\n", buf->name);
        printf("MATH = %d\n", ntohl(buf->math));
        printf("CHINESE = %d\n", ntohl(buf->chinese));
    }

    free(buf);
    close(sd);

    exit(0);
}
