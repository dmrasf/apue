#include "proto.h"
#include <arpa/inet.h>
#include <bits/stdint-uintn.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usages...\n");
        exit(1);
    }

    if (strlen(argv[2]) > NAMEMAX) {
        fprintf(stderr, "Name too len!\n");
        exit(1);
    }

    int sd;
    struct msg_st* buf;
    struct sockaddr_in raddr;
    uint32_t addr;
    int size;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("sockets");
        exit(1);
    }

    size = strlen(argv[2]) + sizeof(struct msg_st) - 1;
    buf = malloc(size);
    if (buf == NULL) {
        fprintf(stderr, "malloc filed\n");
        exit(1);
    }
    memset(buf, '\0', size);

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(RECPORT));
    inet_pton(AF_INET, argv[1], &addr);
    raddr.sin_addr.s_addr = addr;

    strcpy((char*)buf->name, argv[2]);
    buf->math = htonl(rand() % 100);
    buf->chinese = htonl(rand() % 100);

    if (sendto(sd, (void*)buf, size, 0, (void*)&raddr, sizeof(raddr)) < 0) {
        perror("sento");
        exit(1);
    }
    puts("OK!");

    free(buf);

    close(sd);

    exit(0);
}
