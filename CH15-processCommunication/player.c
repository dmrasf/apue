#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 320000

int main(void)
{
    pid_t pid;
    int pip;
    int pd[2];
    char buf[BUFSIZE];
    int len;

    if (pipe(pd) < 0) {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        close(pd[1]);
        dup2(pd[0], 0);
        close(pd[0]);
        /*int fd = open("/dev/null", O_RDWR);*/
        /*dup2(fd, 1);*/
        /*dup2(fd, 2);*/

        execl("/usr/bin/mpg123", "mpg123", "-", NULL);
        perror("mpg123");
        exit(1);
    } else {
        close(pd[0]);
        int fd = open("song.mp3", O_RDONLY);
        while ((len = read(fd, buf, BUFSIZE)) > 0) {
            write(pd[1], buf, len);
        }
        wait(NULL);
    }
    exit(0);
}
