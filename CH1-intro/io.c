#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFSIZE 4096

// 不带缓冲
void io_153() {
    int n;
    char buf[BUFFSIZE];

    // n: 读取到的字节数
    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
        // write 返回写入的字节数
        if (write(STDOUT_FILENO, buf, n) != n) {
            err_sys("write error");
        }
    }

    if (n < 0) {
        err_sys("read error");
    }

    return;
}

// 标准io
void io_154() {
    int c;
    while ((c = getc(stdin)) != EOF) {
        printf("(%d)", c);
        if (putc(c, stdout) == EOF) {
            err_sys("output error");
        }
    }

    if (ferror(stdin)) {
        err_sys("input error");
    }
}

int main(int argc, char* argv[])
{
    /*io_153();*/
    io_154();
}
