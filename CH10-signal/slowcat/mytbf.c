#include "mytbf.h"
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

struct mytbf_st {
    int cps;
    int burst;
    int token;
    int pos;
};

static int inited = 0;

static struct mytbf_st* job[MYTBF_MAX];

static int find_free_pos(void)
{
    for (int i = 0; i < MYTBF_MAX; i++)
        if (job[i] == NULL)
            return i;
    return -1;
}

static void alrm_handler(int s, siginfo_t* siginfo, void* unused)
{
    if (siginfo->si_code == SI_KERNEL) {
        for (int i = 0; i < MYTBF_MAX; i++) {
            if (job[i] != NULL) {
                job[i]->token += job[i]->cps;
                if (job[i]->token >= job[i]->burst)
                    job[i]->token = job[i]->burst;
            }
        }
    }
}

static struct sigaction sa_before;
static struct itimerval iti_old;
static struct itimerval iti_new;

static void module_unload(void)
{
    sigaction(SIGALRM, &sa_before, NULL);
    setitimer(ITIMER_REAL, &iti_old, NULL);
    for (int i = 0; i < MYTBF_MAX; i++)
        free(job[i]);
}

static void module_load(void)
{
    atexit(module_unload);

    struct sigaction sa;
    sa.sa_sigaction = alrm_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGALRM, &sa, &sa_before);

    iti_new.it_interval.tv_sec = 1;
    iti_new.it_interval.tv_usec = 0;
    iti_new.it_value.tv_sec = 1;
    iti_new.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &iti_new, &iti_old) < 0) {
        perror("setitimer");
        exit(1);
    }
}

mytbf_t* mytbf_init(int cps, int burst)
{
    int pos = find_free_pos();
    if (pos < 0)
        return NULL;

    struct mytbf_st* mt;
    mt = malloc(sizeof(*mt));
    if (mt == NULL)
        return NULL;

    mt->cps = cps;
    mt->burst = burst;
    mt->token = 0;
    mt->pos = pos;

    job[pos] = mt;

    if (!inited) {
        module_load();
        inited = 1;
    }

    return mt;
}

int mytbf_fetchtoken(mytbf_t* ptr, int size)
{
    if (size <= 0)
        return -EINVAL;

    struct mytbf_st* mt = ptr;

    while (mt->token <= 0)
        pause();

    int n = mt->token < size ? mt->token : size;
    mt->token -= n;

    return n;
}

int mytbf_returntoken(mytbf_t* ptr, int size)
{
    if (size <= 0)
        return -EINVAL;

    struct mytbf_st* mt = ptr;
    mt->token += size;
    if (mt->token > mt->burst)
        mt->token = mt->burst;

    return 0;
}

void mytbf_destroy(mytbf_t* ptr)
{
    struct mytbf_st* mt = ptr;
    job[mt->pos] = NULL;
    free(mt);
}
