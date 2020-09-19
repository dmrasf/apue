#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024

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
        len = read(pd[0], buf, BUFSIZE);
        write(1, buf, len);
        close(pd[0]);
    } else {
        close(pd[0]);
        write(pd[1], "Hello!", 6);
        close(pd[1]);
        wait(NULL);
    }

    exit(0);
}
