#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 4096

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: ./a.out pathname");
        exit(1);
    }

    int fd1, fd2;
    if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
        perror("open path1 error");
        exit(1);
    }
    if ((fd2 = open(argv[1], O_RDWR | O_CREAT, 0600)) < 0) {
        close(fd1);
        perror("open path2 error");
        exit(1);
    }

    // seek1 下一行第一个字符位置
    // seek2 那一行第一个字符位置
    off_t seek1 = 0, seek2 = 0;

    FILE *fp = NULL;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen() error");
        exit(1);
    }
    // 获取换行符位置
    char *linebuf = NULL;
    size_t bufsize = 0;
    int line = 2;
    while (line--) {
        ssize_t linesize = 0;
        linesize = getline(&linebuf, &bufsize, fp);
        if (linesize < 0) {
            perror("getline error");
            exit(1);
        } else {
            seek2 += linesize;
        }
    }
    seek1 = seek2 + getline(&linebuf, &bufsize, fp);
    fclose(fp);

    seek1 = lseek(fd1, seek1, SEEK_SET);
    seek2 = lseek(fd2, seek2, SEEK_SET);

    printf("seek1 = %ld\n", seek1);
    printf("seek2 = %ld\n", seek2);

    // 从那一行开始写入的字符数
    off_t count = seek2;

    char buf[BUFSIZE];
    int cnt, ret, pos;

    while ((cnt = read(fd1, buf, BUFSIZE)) > 0) {
        pos = 0;
        while (cnt > 0) {
            if ((ret = write(fd2, buf + pos, cnt)) < 0) {
                perror("write()");
                exit(1);
            }
            count += ret;
            cnt -= ret;
        }
    }

    close(fd1);
    close(fd2);

    // 截断去那一行的长度
    if (truncate(argv[1], count) == -1) {
        perror("truncate error\n");
        exit(1);
    }

    exit(0);
}
