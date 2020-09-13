#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define L 300000000
#define R 300000200
#define N 3

int main(void)
{
    int mark, i, j, n;
    pid_t pid;

    for (n = 0; n < N; n++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            // 交叉
            for (i = L + n; i < R; i += N) {
                mark = 1;
                for (j = 2; j < i / 2; j++) {
                    if (i % j == 0) {
                        mark = 0;
                        break;
                    }
                }
                if (mark) {
                    printf("[%d]: %d is primer\n", n, i);
                }
            }
            exit(0);
        }
    }

    for (n = 0; n < N; n++) {
        printf("%d\n", wait(NULL));
    }

    exit(0);
}
