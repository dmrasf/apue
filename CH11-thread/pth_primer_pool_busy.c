#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define L 300000000
#define R 300002000
#define N 10

struct pth_i {
    int i;
};

static int task = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void* thread_primer(void* p)
{
    int j, i, mark;

    while (1) {
        pthread_mutex_lock(&mut);
        while (task == 0) {
            pthread_mutex_unlock(&mut);
            sched_yield();
            pthread_mutex_lock(&mut);
        }
        if (task == -1) {
            pthread_mutex_unlock(&mut);
            break;
        }
        i = task;
        task = 0;
        pthread_mutex_unlock(&mut);

        mark = 1;
        for (j = 2; j < i / 2; j++) {
            if (i % j == 0) {
                mark = 0;
                break;
            }
        }
        if (mark) {
            printf("[%d]: %d is primer\n", ((struct pth_i*)p)->i, i);
        }
    }

    pthread_exit(p);
}

int main(void)
{
    int n, i;
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

    for (i = L; i < R; i++) {
        pthread_mutex_lock(&mut);
        while (task != 0) {
            pthread_mutex_unlock(&mut);
            sched_yield();
            pthread_mutex_lock(&mut);
        }
        task = i;
        pthread_mutex_unlock(&mut);
    }

    pthread_mutex_lock(&mut);
    while (task != 0) {
        pthread_mutex_unlock(&mut);
        sched_yield();
        pthread_mutex_lock(&mut);
    }
    task = -1;
    pthread_mutex_unlock(&mut);

    for (n = 0; n < N; n++) {
        pthread_join(tids[n], ptr);
        free(ptr);
    }

    pthread_mutex_destroy(&mut);

    exit(0);
}
