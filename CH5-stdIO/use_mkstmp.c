#include "../apue.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void make_temp(char* template);

int main(void)
{
    char good_template[] = "/tmp/dirXXXXXX";
    char* bad_template = "/tmp/dirXXXXXX";

    printf("trying to create first temp file...\n");
    make_temp(good_template);
    printf("trying to create second temp file...\n");
    // char*定义的是在栈中分配的一个指针，指向静态区分配了的一块常量区，无法修改内容
    make_temp(bad_template);

    exit(0);
}

void make_temp(char* template)
{
    int fd;
    struct stat sbuf;

    if ((fd = mkstemp(template)) < 0)
        err_sys("can't create temp file");
    printf("temp name = %s\n", template);
    close(fd);
    if (stat(template, &sbuf) < 0) {
        if (errno == ENOENT)
            printf("file doesn't exist\n");
        else
            err_sys("stat failes");
    } else {
        printf("file exists\n");
        unlink(template);
    }
}
