#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

int main(void)
{
    key_t key;
    int msgid;
    struct msg_st msg;

    key = ftok(KEYPATH, KEYPROJ);
    if (key < 0) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, 0);
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    msg.chinese = 100;
    msg.math = 100;
    strcpy(msg.name, "Tom");

    if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
        perror("msgsnd");
        exit(1);
    }
    puts("Done!");

    exit(0);
}
