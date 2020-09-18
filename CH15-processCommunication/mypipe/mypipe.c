#include "mypipe.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct mypipe_st {
    int head;
    int tail;
    char data[PIPESIZE];
    int datasize;
    pthread_mutex_t mut;
    pthread_cond_t cond;
    unsigned int count_read;
    unsigned int count_write;
};

mypipe_t* MyPipeInit()
{
    struct mypipe_st* mst;
    mst = malloc(sizeof(*mst));
    if (mst == NULL)
        return NULL;

    mst->head = 0;
    mst->tail = 0;
    mst->datasize = 0;
    mst->count_read = 0;
    mst->count_write = 0;
    pthread_mutex_init(&mst->mut, NULL);
    pthread_cond_init(&mst->cond, NULL);

    return mst;
}

int MyPipeRegister(mypipe_t* ptr, int opmap)
{
    if (opmap == 0)
        return -1;

    struct mypipe_st* mst = ptr;

    pthread_mutex_lock(&mst->mut);
    if (opmap & MYPIPE_READ)
        mst->count_read++;
    if (opmap & MYPIPE_WRITE)
        mst->count_write++;
    pthread_cond_broadcast(&mst->cond);
    while (mst->count_read == 0 || mst->count_write == 0)
        pthread_cond_wait(&mst->cond, &mst->mut);
    pthread_mutex_unlock(&mst->mut);
    return 0;
}

int MyPipeUnRegister(mypipe_t* ptr, int opmap)
{
    if (opmap == 0)
        return -1;

    struct mypipe_st* mst = ptr;

    pthread_mutex_lock(&mst->mut);
    if (opmap & MYPIPE_READ)
        mst->count_read--;
    if (opmap & MYPIPE_WRITE)
        mst->count_write--;
    pthread_cond_wait(&mst->cond, &mst->mut);
    pthread_mutex_unlock(&mst->mut);
    return 0;
}

size_t MyPipeRead(mypipe_t* ptr, void* buf, size_t size)
{
    struct mypipe_st* mst = ptr;
    int i;

    pthread_mutex_lock(&mst->mut);
    while (mst->datasize <= 0 && mst->count_write > 0)
        pthread_cond_wait(&mst->cond, &mst->mut);
    if (mst->datasize <= 0 && mst->count_write <= 0) {
        pthread_mutex_unlock(&mst->mut);
        return 0;
    }
    for (i = 0;; i++) {
        if (mst->datasize <= 0)
            break;
        *((char*)buf + i) = mst->data[mst->head];
        mst->head = (mst->head + 1) % PIPESIZE;
        mst->datasize--;
    }
    pthread_cond_broadcast(&mst->cond);
    pthread_mutex_unlock(&mst->mut);

    return i;
}

size_t MyPipeWrite(mypipe_t* ptr, const void* buf, size_t size)
{
    struct mypipe_st* mst = ptr;
    int i;

    pthread_mutex_lock(&mst->mut);
    while (mst->datasize >= PIPESIZE && mst->count_read > 0)
        pthread_cond_wait(&mst->cond, &mst->mut);
    if (mst->datasize >= PIPESIZE && mst->count_read <= 0) {
        pthread_mutex_unlock(&mst->mut);
        return 0;
    }
    for (i = 0; i < size; i++) {
        if (mst->datasize >= PIPESIZE)
            break;
        mst->data[mst->tail] = *((char*)buf + i);
        mst->tail = (mst->tail + 1) % PIPESIZE;
        mst->datasize++;
    }
    pthread_cond_broadcast(&mst->cond);
    pthread_mutex_unlock(&mst->mut);

    return i;
}

int MyPipeDestroy(mypipe_t* ptr)
{
    struct mypipe_st* mst = ptr;

    pthread_mutex_destroy(&mst->mut);
    pthread_cond_destroy(&mst->cond);
    free(mst);

    return 0;
}
