#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#define THR_NUM 4

static pthread_mutex_t mut[THR_NUM];

struct pth_i {
    int i;
};

static void* thread_func(void* ptr)
{
    int n = ((struct pth_i*)ptr)->i;

    while (1) {
        pthread_mutex_lock(mut + n);
        printf("%d", n);
        pthread_mutex_unlock(mut + (n + 1) % 4);
    }

    pthread_exit(NULL);
}

int main(void)
{
    int i, err;
    pthread_t tids[THR_NUM], tid;
    struct pth_i* p;

    for (i = 0; i < THR_NUM; i++) {
        pthread_mutex_init(&mut[i], NULL);
        pthread_mutex_lock(mut + i);

        p = malloc(sizeof(*p));
        p->i = i;
        if ((err = pthread_create(&tid, NULL, thread_func, p))) {
            fprintf(stderr, "%s\n", strerror(err));
        }
        tids[i] = tid;
    }

    pthread_mutex_unlock(mut + 0);

    alarm(1);

    for (i = 0; i < THR_NUM; i++) {
        pthread_mutex_destroy(mut + i);
        pthread_join(tids[i], NULL);
    }

    exit(0);
}
