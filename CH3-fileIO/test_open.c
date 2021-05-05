// 测试open、openat函数

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int fd;
    fd = open("./mycp.c", O_RDONLY);
    printf("%d\n", fd);

    int dir_fd;
    DIR* dir = opendir("../CH3-fileIO");
    dir_fd = dirfd(dir);

    int fd2;
    fd2 = openat(dir_fd, "mycp.c", O_RDONLY);
    printf("%d\n", fd2);

    return 0;
}
