#include "proto.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Uasge...\n");
        exit(1);
    }

    int sd;
    struct sockaddr_in raddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        exit(1);
    }

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, argv[1], &raddr.sin_addr.s_addr);

    if (connect(sd, (void*)&raddr, sizeof(raddr)) < 0) {
        perror("connect");
        exit(1);
    }

    FILE* fp;
    fp = fdopen(sd, "r");
    if (fp == NULL) {
        perror("fdopen");
        exit(1);
    }

    long long stamp;

    if (fscanf(fp, "%lld\r\n", &stamp) < 1)
        fprintf(stderr, "fscanf\n");
    else
        printf("%lld\n", stamp);

    fclose(fp);
    exit(0);
}
