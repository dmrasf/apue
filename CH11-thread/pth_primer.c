#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define L 300000000
#define R 300000200

struct pth_i {
    int i;
};

static void* thread_primer(void* p)
{
    int mark = 1, j;
    int num = ((struct pth_i*)p)->i;
    for (j = 2; j < num / 2; j++) {
        if (num % j == 0) {
            mark = 0;
            break;
        }
    }
    if (mark) {
        printf("%d is primer\n", num);
    }
    pthread_exit(p);
}

int main(void)
{
    int i;
    pthread_t tids[R - L + 1];
    int err;
    struct pth_i* p;
    void* ptr;

    for (i = L; i < R; i++) {
        p = malloc(sizeof(*p));
        if (p == NULL) {
            perror("malloc");
            exit(1);
        }
        p->i = i;

        pthread_t tid;
        if ((err = pthread_create(&tid, NULL, thread_primer, p))) {
            fprintf(stderr, "%s\n", strerror(err));
            exit(1);
        }
        tids[i - L] = tid;
    }

    for (i = L; i < R; i++) {
        pthread_join(tids[i - L], &ptr);
        free(ptr);
    }

    exit(0);
}
