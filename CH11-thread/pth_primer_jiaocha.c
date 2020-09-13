#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define L 300000000
#define R 300000200
#define N 3

struct pth_i {
    int i;
};

static void* thread_primer(void* p)
{
    int i, j, mark;
    int n = ((struct pth_i*)p)->i;

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
    pthread_exit(p);
}

int main(void)
{
    int n;
    pthread_t tids[N];
    pthread_t tid;
    int err;
    void* ptr;
    struct pth_i* p;

    for (n = 0; n < N; n++) {
        p = malloc(sizeof(*p));
        if (p == NULL) {
            perror("malloc");
            exit(1);
        }
        p->i = n;

        if ((err = pthread_create(&tid, NULL, thread_primer, p))) {
            fprintf(stderr, "%s\n", strerror(err));
            exit(1);
        }

        tids[n] = tid;
    }

    for (n = 0; n < N; n++) {
        pthread_join(tids[n], ptr);
        free(ptr);
    }

    exit(0);
}
