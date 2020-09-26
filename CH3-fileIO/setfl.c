#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void get_flag(int fd);
void set_fl(int fd, int flags);

int main(int argc, char* argv[])
{
    int fd;

    if (argc != 2) {
        fprintf(stderr, "Usage\n");
        exit(1);
    }

    fd = open(argv[1], O_WRONLY,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    // 查看flag
    get_flag(fd);
    // 添加append
    set_fl(fd, O_APPEND);
    get_flag(fd);

    return 0;
}

void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        perror("fcntl");
        exit(1);
    }

    val |= flags; /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0) {
        perror("fcntl");
        exit(1);
    }
}

void get_flag(int fd)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        perror("fcntl");
        exit(1);
    }

    switch (val & O_ACCMODE) {
    case O_RDONLY:
        printf("read only\n");
        break;

    case O_WRONLY:
        printf("write only\n");
        break;

    case O_RDWR:
        printf("read write\n");
        break;

    default:
        exit(1);
    }

    printf("val: %#x\n", val);
    printf("O_APPEND: %#x\n", O_APPEND);
    printf("val & O_APPEND: %#x\n", val & O_APPEND);

    if (val & O_APPEND)
        printf(", append\n");
    if (val & O_NONBLOCK)
        printf(", nonblocking\n");
    if (val & O_SYNC)
        printf(", synchronous writes\n");
}
