#include "proto.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

static int msgid;
static __sighandler_t sig_before;

static void ipcrm()
{
    msgctl(msgid, IPC_RMID, NULL);
    signal(SIGINT, sig_before);
}

int main(void)
{
    key_t key;
    struct msg_st msg;

    if ((sig_before = signal(SIGINT, ipcrm)) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    key = ftok(KEYPATH, KEYPROJ);
    if (key < 0) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        if (msgrcv(msgid, &msg, sizeof(msg), 0, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }
        printf("name = %s\n", msg.name);
        printf("math = %d\n", msg.math);
        printf("chinese = %d\n", msg.chinese);
    }

    exit(0);
}
