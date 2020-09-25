#include "proto.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define IPSTRSIZE 40
#define BUFSIZE 1024
#define FORKNUM 10

static int sd;
static __sighandler_t hander_before;

static void server(int sd);

static void sig_hander(int s)
{
    close(sd);
    signal(SIGINT, hander_before);
    exit(0);
}

int main(void)
{
    int newsd, pidsd;
    struct sockaddr_in laddr, raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];
    int pid;

    hander_before = signal(SIGINT, sig_hander);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        fprintf(stderr, "socket failed!");
        exit(1);
    }

    int val;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr);

    if (bind(sd, (void*)&laddr, sizeof(laddr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(sd, 1) < 0) {
        perror("listen");
        exit(1);
    }
    raddr_len = sizeof(raddr);

    while (1) {
        newsd = accept(sd, (void*)&raddr, &raddr_len);
        if (newsd < 0) {
            perror("accept");
            exit(1);
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            close(sd);
            inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ipstr, IPSTRSIZE);
            printf("=========FROM: %s:%d========\n", ipstr, ntohs(raddr.sin_port));

            server(newsd);
            close(newsd);
            exit(0);
        }
        close(newsd);
    }

    exit(0);
}

static void server(int sd)
{
    char buf[BUFSIZE];
    int len = 0;

    len = sprintf(buf, "%lld\r\n", (long long)time(NULL));

    if (send(sd, buf, len, 0) < 0) {
        perror("send");
        exit(1);
    }

    sleep(10);

    puts("Done!");
}
