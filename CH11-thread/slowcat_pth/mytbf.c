#include "mytbf.h"
#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

struct mytbf_st {
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut_token;
};

static int inited = 0;

static struct mytbf_st* job[MYTBF_MAX];
static pthread_t tid;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

static int find_free_pos_unlocked(void)
{
    for (int i = 0; i < MYTBF_MAX; i++)
        if (job[i] == NULL)
            return i;
    return -1;
}

static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;

static void* alrm_handler(void* ptr)
{
    struct timespec rq;
    rq.tv_sec = 0;
    rq.tv_nsec = 10000000;

    while (1) {
        pthread_mutex_lock(&mut_job);
        for (int i = 0; i < MYTBF_MAX; i++) {
            if (job[i] != NULL) {
                pthread_mutex_lock(&job[i]->mut_token);
                job[i]->token += job[i]->cps;
                if (job[i]->token >= job[i]->burst)
                    job[i]->token = job[i]->burst;
                pthread_mutex_unlock(&job[i]->mut_token);
            }
        }
        pthread_mutex_unlock(&mut_job);
        nanosleep(&rq, NULL);
    }
}

static void module_unload(void)
{
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    for (int i = 0; i < MYTBF_MAX; i++) {
        if (job[i] != NULL) {
            pthread_mutex_destroy(&job[i]->mut_token);
            free(job[i]);
        }
    }
    pthread_mutex_destroy(&mut_job);
}

static void module_load(void)
{
    /*pthread_cleanup_push(module_unload, NULL);*/
    int err;
    if ((err = pthread_create(&tid, NULL, alrm_handler, NULL))) {
        fprintf(stderr, "%s\n", strerror(err));
        exit(1);
    }
    /*pthread_exit(NULL);*/
    /*pthread_cleanup_pop(1);*/

    atexit(module_unload);
}

mytbf_t* mytbf_init(int cps, int burst)
{
    pthread_once(&init_once, module_load);

    struct mytbf_st* mt;
    mt = malloc(sizeof(*mt));
    if (mt == NULL)
        return NULL;
    mt->cps = cps;
    mt->burst = burst;
    mt->token = 0;
    pthread_mutex_init(&mt->mut_token, NULL);

    pthread_mutex_lock(&mut_job);
    int pos = find_free_pos_unlocked();
    if (pos < 0) {
        pthread_mutex_unlock(&mut_job);
        free(mt);
        return NULL;
    }

    mt->pos = pos;
    job[pos] = mt;
    pthread_mutex_unlock(&mut_job);

    return mt;
}

int mytbf_fetchtoken(mytbf_t* ptr, int size)
{
    if (size <= 0)
        return -EINVAL;
    struct mytbf_st* mt = ptr;

    pthread_mutex_lock(&mt->mut_token);
    while (mt->token <= 0) {
        pthread_mutex_unlock(&mt->mut_token);
        sched_yield();
        pthread_mutex_lock(&mt->mut_token);
    }
    int n = mt->token < size ? mt->token : size;
    mt->token -= n;
    pthread_mutex_unlock(&mt->mut_token);

    return n;
}

int mytbf_returntoken(mytbf_t* ptr, int size)
{
    if (size <= 0)
        return -EINVAL;

    struct mytbf_st* mt = ptr;

    pthread_mutex_lock(&mt->mut_token);
    mt->token += size;
    if (mt->token > mt->burst)
        mt->token = mt->burst;
    pthread_mutex_unlock(&mt->mut_token);

    return 0;
}

void mytbf_destroy(mytbf_t* ptr)
{
    struct mytbf_st* mt = ptr;
    pthread_mutex_lock(&mut_job);
    job[mt->pos] = NULL;
    free(mt);
    pthread_mutex_unlock(&mut_job);
}
