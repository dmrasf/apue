#include "proto.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <bits/types/sigset_t.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// idel 的限制
#define MINSERVER 5
#define MAXSERVER 10

// 总服务数
#define CLIENT 20

// 子进程通知父进程改变状态
#define SIG_NOTIFY SIGUSR1

#define BUFSIZE 100

enum {
    STATE_IDLE = 0,
    STATE_BUSY
};

struct server_st {
    pid_t pid;
    int state;
    /*int reuse;*/
};

static struct server_st* serverpool;
static int idle_count, busy_count;
static int sd;

static void usr1_handler(int s)
{
    return;
}

static void server_run(int slot)
{
    int ppid = getppid();
    struct sockaddr_in raddr;
    socklen_t socklen;
    int client_sd;

    while (1) {
        serverpool[slot].state = STATE_IDLE;
        printf("slot = %d [idel] state = %d\n", slot, serverpool[slot].state);
        kill(ppid, SIG_NOTIFY);

        client_sd = accept(sd, (void*)&raddr, &socklen);
        if (client_sd < 0) {
            if (errno != EINTR || errno != EAGAIN) {
                perror("accept failed");
                exit(1);
            }
        }

        serverpool[slot].state = STATE_BUSY;
        printf("slot = %d [busy] state = %d\n", slot, serverpool[slot].state);
        kill(ppid, SIG_NOTIFY);

        sleep(2);

        char buf[BUFSIZE];
        int len = 0;
        len = snprintf(buf, BUFSIZE, "%lld\r\n", (long long)time(NULL));
        if (send(client_sd, buf, len, 0) < 0) {
            perror("send");
            exit(1);
        }

        close(client_sd);
    }
    return;
}

static int add_1_server()
{
    int pid;
    int i;

    if (idle_count + busy_count >= CLIENT) {
        return -1;
    }

    for (i = 0; i < CLIENT; i++)
        if (serverpool[i].pid == -1)
            break;

    serverpool[i].state = STATE_IDLE;
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(1);
    }
    if (pid == 0) {
        server_run(i);
        exit(0);
    } else {
        serverpool[i].pid = pid;
    }
    return 0;
}

static int del_1_server()
{
    int i;

    if (idle_count == 0)
        return -1;

    for (i = 0; i < CLIENT; i++)
        if (serverpool[i].pid != -1 && serverpool[i].state == STATE_IDLE) {
            kill(serverpool[i].pid, SIGTERM);
            serverpool[i].pid = -1;
            idle_count--;
            break;
        }

    return 0;
}

static void scan_pool()
{
    int i;
    int idle = 0, busy = 0;

    for (i = 0; i < CLIENT; i++) {
        /*printf("pid = %d, state = %d\n", serverpool[i].pid, serverpool[i].state);*/
        /*fflush(NULL);*/

        if (serverpool[i].pid == -1)
            continue;

        // 判断进程是否存在
        if (kill(serverpool[i].pid, 0)) {
            serverpool[i].pid = -1;
            continue;
        }

        if (serverpool[i].state == STATE_IDLE)
            idle++;
        else if (serverpool[i].state == STATE_BUSY)
            busy++;
        else {
            perror("scan fool");
            abort();
        }
    }
    idle_count = idle;
    busy_count = busy;
}

static void printf_pool()
{
    int i;

    for (i = 0; i < CLIENT; i++) {
        if (serverpool[i].pid == -1) {
            putchar(' ');
        } else if (serverpool[i].state == STATE_IDLE) {
            putchar('.');
        } else {
            putchar('x');
        }
    }
    putchar('\n');
}

int main(void)
{
    struct sigaction sa, oldsa;
    int i;
    int val;
    struct sockaddr_in laddr;

    // 必须使用共享内存，否则子进程与父进程间无法沟通，for()后会产生新的空间
    serverpool = mmap(NULL, sizeof(struct server_st) * CLIENT, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (i = 0; i < CLIENT; i++) {
        serverpool[i].pid = -1;
    }

    // 子进程自己收尸
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa, &oldsa);

    // 接受子进程通知并处理
    sa.sa_handler = usr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &oldsa);

    sigset_t set, oldset;
    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &set, &oldset);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket failed");
        exit(1);
    }

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("setsocket failed");
        exit(1);
    }

    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr);
    laddr.sin_family = AF_INET;
    if (bind(sd, (void*)&laddr, sizeof(laddr)) < 0) {
        perror("error failed");
        exit(1);
    }

    // 只要大于最大上限就行
    if (listen(sd, 100) < 0) {
        perror("listen failed");
        exit(1);
    }

    for (i = 0; i < MINSERVER; i++) {
        add_1_server();
    }

    while (1) {
        sigsuspend(&oldset);

        scan_pool();

        if (idle_count > MAXSERVER) {
            for (i = 0; i < idle_count - MAXSERVER; i++) {
                del_1_server();
            }
        } else if (idle_count < MINSERVER) {
            for (i = 0; i < MINSERVER - idle_count; i++) {
                add_1_server();
            }
        }

        printf_pool();
    }

    sigprocmask(SIG_UNBLOCK, &oldset, NULL);

    exit(0);
}
