#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 20

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    int fd, newfd;
    char buf[BUFSIZE];

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    newfd = dup(fd);
    if (newfd < 0) {
        perror("dup error");
        exit(1);
    }

    // 若输出相同则指向不同的文件表
    lseek(fd, 20, SEEK_CUR);
    read(fd, buf, BUFSIZE);
    puts(buf);
    lseek(newfd, 20, SEEK_CUR);
    read(newfd, buf, BUFSIZE);
    puts(buf);

    exit(0);
}
