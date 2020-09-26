#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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

    if ((fd = creat("file.hole", O_RDWR)) < 0) {
        perror("creat");
        exit(1);
    }
    if (write(fd, buf1, 10) != 10) {
        perror("write");
        exit(1);
    }
    if (lseek(fd, 20, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    if (write(fd, buf2, 10) != 10) {
        perror("write");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    holeSeek();
    return 0;
}
