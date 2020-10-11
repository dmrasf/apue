#include "../apue.h"
#include <stdio.h>
#include <sys/stat.h>

void my_setbuf(FILE*, char*);

int main(void)
{

    return 0;
}

void my_setbuf(FILE* fp, char* buf)
{
    // 不带缓冲
    if (buf == NULL || fp == stderr)
        if (setvbuf(fp, NULL, _IONBF, 0) != 0)
            err_sys("_IONBF error");

    int fd;
    struct stat st;

    fd = fileno(fp);
    if (fstat(fd, &st) == -1)
        err_sys("get stat error");

    if (S_ISFIFO(st.st_mode) || S_ISSOCK(st.st_mode)
        || S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)) {
        if (setvbuf(fp, buf, _IOLBF, BUFSIZ) != 0)
            err_sys("_IOLBF error");
    } else {
        if (setvbuf(fp, buf, _IOFBF, BUFSIZ) != 0)
            err_sys("_IOFBF error");
    }
}
