#include "../apue.h"
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int val, fd;

    if (argc != 2)
        err_quit("usage: a.out <descriptor#>");

    fd = open(argv[1], O_WRONLY,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl error for fd %d", atoi(argv[1]));

    printf("val: %#x\n", val);
    printf("O_ACCMODE: %x\n", O_ACCMODE);
    printf("O_SYNC: %#x\n", 04010000);
    printf("val & O_ACCMODE: %x\n", val & O_ACCMODE);
    printf("val & O_SYNC: %#x\n", val & O_SYNC);

    switch (val & O_ACCMODE) {
    case O_RDONLY:
        printf("read only");
        break;

    case O_WRONLY:
        printf("write only");
        break;

    case O_RDWR:
        printf("read write");
        break;

    default:
        err_dump("unknown access mode");
    }

    if (val & O_APPEND)
        printf(", append");
    if (val & O_NONBLOCK)
        printf(", nonblocking");
    if (val & O_SYNC)
        printf(", synchronous writes");

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC)
        printf(", synchronous writes");
#endif

    putchar('\n');
    exit(0);
}
