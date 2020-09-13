#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;

    printf("begin\n");
    fflush(NULL);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // 只有在出错的时候才返回并设置errno
        execl("/bin/sleep", "sleep", "100", NULL);
        perror("execl()");
        exit(1);
    }

    // 等待子进程结束
    wait(NULL);

    printf("end\n");

    exit(0);
}
