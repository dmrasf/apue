#include "mysem.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

struct mytbf_st {
    int value;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

mysem_t* mysem_init(int n)
{
    struct mytbf_st* mst;
    if ((mst = malloc(sizeof(*mst))) == NULL)
        return NULL;

    mst->value = n;
    pthread_mutex_init(&mst->mut, NULL);
    pthread_cond_init(&mst->cond, NULL);

    return mst;
}
int mysem_add(mysem_t* ptr, int n)
{
    if (n <= 0)
        return -1;

    struct mytbf_st* mst = ptr;
    pthread_mutex_lock(&mst->mut);
    mst->value += n;
    pthread_mutex_unlock(&mst->mut);
    return n;
}

int mysem_sub(mysem_t* ptr, int n)
{
    struct mytbf_st* mst = ptr;

    if (n <= 0 || n > mst->value)
        return -1;

    pthread_mutex_lock(&mst->mut);
    while (mst->value < n) {
        pthread_cond_wait(&mst->cond, &mst->mut);
    }
    mst->value -= n;
    pthread_mutex_unlock(&mst->mut);

    return n;
}

void mytbf_destroy(mysem_t* ptr)
{
    struct mytbf_st* mst = ptr;

    pthread_mutex_destroy(&mst->mut);
    pthread_cond_destroy(&mst->cond);
    free(mst);
}
