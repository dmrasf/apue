#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 20

int main(int argc, char* argv[])
{
    int fd;
    char buf[BUFSIZE] = "12345678";

    fd = open("test_file", O_RDWR | O_APPEND | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    write(fd, buf, BUFSIZE);
    lseek(fd, 0, SEEK_SET);
    write(fd, buf, BUFSIZE);
    exit(0);
}
