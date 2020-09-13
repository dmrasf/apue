#include "../apue.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void isSeek()
{
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1) {
        printf("can't seek\n");
    } else {
        printf("seek ok\n");
    }
}

void holeSeek()
{
    char buf1[] = "abcdefghij";
    char buf2[] = "ABCDEFGHIJ";
    int fd;

    if ((fd = creat("file.hole", FILE_MODE)) < 0)
        err_sys("creat error");
    if (write(fd, buf1, 10) != 10)
        err_sys("buf1 write error");
    if (lseek(fd, 20, SEEK_SET) == -1)
        err_sys("lseek error");
    if (write(fd, buf2, 10) != 10)
        err_sys("buf2 write error");
}

int main(int argc, char* argv[])
{
    holeSeek();
    return 0;
}
