#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#define THR_NUM 4

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condt = PTHREAD_COND_INITIALIZER;
static int num = 0;

struct pth_i {
    int i;
};

static void* thread_func(void* ptr)
{
    int n = ((struct pth_i*)ptr)->i;

    while (1) {
        pthread_mutex_lock(&mut);
        while (n != num) {
            // 若不等于自己，解mut等待condt
            pthread_cond_wait(&condt, &mut);
        }
        printf("%d", n);
        num = (num + 1) % THR_NUM;
        // 若等于自己，给所以线程发 condt
        pthread_cond_broadcast(&condt);
        pthread_mutex_unlock(&mut);
    }

    pthread_exit(NULL);
}

int main(void)
{
    int i, err;
    pthread_t tids[THR_NUM], tid;
    struct pth_i* p;

    pthread_mutex_lock(&mut);
    for (i = 0; i < THR_NUM; i++) {
        p = malloc(sizeof(*p));
        p->i = i;
        if ((err = pthread_create(&tid, NULL, thread_func, p))) {
            fprintf(stderr, "%s\n", strerror(err));
        }
        tids[i] = tid;
    }
    pthread_mutex_unlock(&mut);

    alarm(1);

    for (i = 0; i < THR_NUM; i++)
        pthread_join(tids[i], NULL);

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&condt);

    exit(0);
}
