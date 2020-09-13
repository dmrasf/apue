#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void* thread_start(void* ptr)
{
    puts("thread");
    pthread_exit(NULL);
}

int main(void)
{
    puts("Begin");

    pthread_t tid;
    int err;

    if ((err = pthread_create(&tid, NULL, thread_start, NULL))) {
        fprintf(stderr, "%s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);

    puts("End");
    exit(0);
}
