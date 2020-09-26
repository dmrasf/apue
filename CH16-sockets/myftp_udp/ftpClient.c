#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 1024

static int msgid;
static char path[PATHSIZE];

static void msg_creat();

enum {
    STATE_Rc = 1,
    STATE_Out,
    STATE_Sn,
    STATE_Ex,
    STATE_T
};

struct fsm_st {
    int state;
    char* errStr;
    char buf[BUFSIZE];
    int len;
    int writePos;
};

static void fsm_driver(struct fsm_st*);

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    msg_creat();

    strcpy(path, argv[1]);

    struct fsm_st fsm;

    fsm.state = STATE_Sn;

    while (fsm.state != STATE_T)
        fsm_driver(&fsm);

    exit(0);
}

static void fsm_driver(struct fsm_st* fsm)
{
    union msg_un msg;
    int writeLen = 0;

    switch (fsm->state) {
    case STATE_Sn:
        msg.mtype = PATHTYPE;
        strcpy(msg.pathmsg.path, path);
        if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
            fsm->errStr = "msgsnd path";
            fsm->state = STATE_Ex;
        } else
            fsm->state = STATE_Rc;
        break;
    case STATE_Rc:
        if (msgrcv(msgid, &msg, sizeof(msg), 0, 0) < 0) {
            fsm->errStr = "recive";
            fsm->state = STATE_Ex;
        } else {
            if (msg.mtype == EOTTYPE) {
                fsm->state = STATE_T;
            } else {
                strncpy(fsm->buf, msg.datamsg.data, msg.datamsg.datalen);
                fsm->len = msg.datamsg.datalen;
                fsm->state = STATE_Out;
                fsm->writePos = 0;
            }
        }
        break;
    case STATE_Out:
        writeLen = write(1, fsm->buf + fsm->writePos, fsm->len);
        if (writeLen < 0) {
            fsm->errStr = "write";
            fsm->state = STATE_Ex;
        } else if (writeLen == fsm->len)
            fsm->state = STATE_Rc;
        else {
            fsm->state = STATE_Out;
            fsm->len -= writeLen;
            fsm->writePos += writeLen;
        }
        break;
    case STATE_Ex:
        perror(fsm->errStr);
        fsm->state = STATE_T;
        break;
    case STATE_T:
        break;
    default:
        break;
    }
}

static void msg_creat()
{
    key_t key;

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
}
