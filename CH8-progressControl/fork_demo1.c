#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define L 300000000
#define R 300000200

int main(void)
{
    int mark, i, j;

    for (i = L; i < R; i++) {
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
    }
    exit(0);
}
