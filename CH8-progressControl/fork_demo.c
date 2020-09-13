#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define L 300000000
#define R 300000200

int main(void)
{
    int mark, i, j;
    pid_t pid;

    for (i = L; i < R; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            mark = 1;
            for (j = 2; j < i / 2; j++) {
                if (i % j == 0) {
                    mark = 0;
                    break;
                }
            }
            if (mark) {
                printf("%d is primer\n", i);
            }
            exit(0);
        }
    }
    for (i = L; i < R; i++) {
        printf("%d\n", wait(NULL));
    }
    exit(0);
}
