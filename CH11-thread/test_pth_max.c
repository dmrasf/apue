#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void* func(void* p)
{
    return NULL;
}

int main(void)
{
    pthread_t mid;
    int i, err;
    struct timespec tsp;

    for (i = 0;; i++) {
        tsp.tv_nsec = 5000000;
        tsp.tv_sec = 0;
        err = pthread_create(&mid, NULL, func, NULL);
        if (err) {
            fprintf(stderr, "%s\n", strerror(err));
            break;
        }
        nanosleep(&tsp, NULL);
    }

    printf("max = %d\n", i);

    exit(0);
}
