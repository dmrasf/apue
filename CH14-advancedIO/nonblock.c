#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"

#define BUFSIZE 1024

enum {
    STATE_R = 1,
    STATE_W,
    STATE_Ex,
    STATE_T
};

struct fsm_st {
    int state;
    int sfd;
    int dfd;
    char buf[BUFSIZE];
    int len;
    int writePos;
    char *errStr;
};

static void relay(int fd1, int fd2);
static void fsm_driver(struct fsm_st*);

int main(void)
{
    int fd1, fd2;

    if ((fd1 = open(TTY1, O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
    write(fd1, "TTY11\n", 5);
    if ((fd2 = open(TTY2, O_RDWR | O_NONBLOCK)) < 0) {
        close(fd1);
        perror("open");
        exit(1);
    }
    write(fd2, "TTY12\n", 5);

    relay(fd1, fd2);

    close(fd1);
    close(fd2);

    exit(0);
}

static void relay(int fd1, int fd2)
{
    int fd_save1, fd_save2;

    fd_save1 = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, fd_save1 | O_NONBLOCK);
    fd_save2 = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd_save2 | O_NONBLOCK);

    struct fsm_st fsm12, fsm21;

    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    while (fsm12.state != STATE_T || fsm21.state != STATE_T) {
        fsm_driver(&fsm12);
        fsm_driver(&fsm21);
    }

    fcntl(fd1, F_SETFL, fd_save1);
    fcntl(fd2, F_SETFL, fd_save2);
}

static void fsm_driver(struct fsm_st* fsm)
{
    int writeLen = 0;

    switch (fsm->state) {
    case STATE_R:
        fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
        if (fsm->len == 0)
            fsm->state = STATE_T;
        else if (fsm->len < 0) {
            if (errno == EAGAIN)
                fsm->state = STATE_R;
            else {
                fsm->state = STATE_Ex;
                fsm->errStr = "read";
            }
        } else {
            fsm->state = STATE_W;
            fsm->writePos = 0;
        }
        break;
    case STATE_W:
        writeLen = write(fsm->dfd, fsm->buf + fsm->writePos, fsm->len);
        if (writeLen < 0)
            if (errno == EAGAIN)
                fsm->state = STATE_W;
            else {
                fsm->state = STATE_Ex;
                fsm->errStr = "write";
            }
        else if (writeLen == fsm->len)
            fsm->state = STATE_R;
        else {
            fsm->state = STATE_W;
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
        abort();
        break;
    }
}
