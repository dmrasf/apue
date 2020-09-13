#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    int fd;
    if ((fd = open("/tmp/out", O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0) {
        perror("open() error");
        exit(1);
    }

    dup2(fd, 1);
    if (fd != 1)
        close(fd);

    puts("hello!");

    exit(0);
}
