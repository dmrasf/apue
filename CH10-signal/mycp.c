#include "../apue.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 4096

int fd1, fd2;

static void closefd(void)
{
    close(fd1);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
        err_quit("usage: ./a.out <file1> <file2>\n");

    do {
        fd1 = open(argv[1], O_RDONLY);
        if (fd1 < 0) {
            if (errno != EINTR) {
                perror("open");
                exit(1);
            }
        }
    } while (fd1 < 0);
    atexit(closefd);
    if ((fd2 = open(argv[2], O_RDWR | O_CREAT, 0600)) < 0) {
        err_quit("creat %s error\n", argv[2]);
    }

    char buf[BUFSIZE];
    int cnt, ret, pos;
    while ((cnt = read(fd1, buf, BUFSIZE)) > 0) {

        /*// 跳过空洞*/
        /*for (int i = 0; i < cnt; i++) {*/
        /*if (buf[i] == '\0') continue;*/
        /*if ((write(fd2, &buf[i], 1)) < 0)*/
        /*err_quit("write %s error\n", argv[2]);*/
        /*}*/

        pos = 0;
        while (cnt > 0) {
            if ((ret = write(fd2, buf + pos, cnt)) < 0) {
                perror("write()");
                exit(1);
            }
            cnt -= ret;
        }
    }

    close(fd1);
    close(fd2);

    exit(0);
}
