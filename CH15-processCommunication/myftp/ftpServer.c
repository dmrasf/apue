#include "proto.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE DATASIZE

static int msgid;
static __sighandler_t sig_before;
static char path[PATHSIZE];

static void ipcrm()
{
    msgctl(msgid, IPC_RMID, NULL);
    signal(SIGINT, sig_before);
}

enum {
    STATE_Rc = 1,
    STATE_Op,
    STATE_In,
    STATE_Sn,
    STATE_Ex,
    STATE_T
};

struct fsm_st {
    int state;
    char* errStr;
    int fd;
    char buf[BUFSIZE];
    int len;
};

static void fsm_driver(struct fsm_st*);
static void msg_create();

int main(void)
{
    msg_create();

    struct fsm_st fsm;

    fsm.len = 0;
    fsm.state = STATE_Rc;

    while (fsm.state != STATE_T) {
        fsm_driver(&fsm);
    }

    exit(0);
}

static void fsm_driver(struct fsm_st* fsm)
{
    union msg_un msg;

    switch (fsm->state) {
    case STATE_Rc:
        // 收取路径出错
        while (1) {
            if (msgrcv(msgid, &msg, sizeof(msg), 0, 0) < 0) {
                fsm->errStr = "recive";
                fsm->state = STATE_Ex;
                break;
            } else {
                // 包不是路径包
                if (msg.mtype == PATHTYPE) {
                    strcpy(path, msg.pathmsg.path);
                    fsm->state = STATE_Op;
                    break;
                }
            }
        }
        break;
    case STATE_Op:
        // 打开文件
        if ((fsm->fd = open(path, O_RDONLY)) < 0) {
            fsm->errStr = "open";
            perror(fsm->errStr);
            msg.eotmsg.mtype = EOTTYPE;
            if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
                fsm->errStr = "open error to msgsnd eot";
                fsm->state = STATE_Ex;
            } else
                fsm->state = STATE_Rc;
        } else
            fsm->state = STATE_In;
        break;
    case STATE_In:
        // 读取文件
        fsm->len = read(fsm->fd, fsm->buf, BUFSIZE);
        if (fsm->len < 0) {
            fsm->errStr = "read";
            fsm->state = STATE_Ex;
        } else
            fsm->state = STATE_Sn;
        break;
    case STATE_Sn:
        // 发送文件
        if (fsm->len == 0) {
            msg.eotmsg.mtype = EOTTYPE;
            if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
                fsm->errStr = "msgsnd eot";
                fsm->state = STATE_Ex;
            } else
                fsm->state = STATE_T;
        } else {
            msg.datamsg.mtype = DATATYPE;
            strcpy(msg.datamsg.data, fsm->buf);
            msg.datamsg.datalen = fsm->len;
            if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
                fsm->errStr = "msgsnd data";
                fsm->state = STATE_Ex;
            } else
                fsm->state = STATE_In;
        }
        break;
    case STATE_Ex:
        perror(fsm->errStr);
        fsm->state = STATE_T;
        break;
    case STATE_T:
        msgctl(msgid, IPC_RMID, NULL);
        break;
    default:
        kill(getpid(), SIGINT);
        break;
    }
}

static void msg_create()
{
    key_t key;

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
}
