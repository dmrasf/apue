#include "anytime.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct anytime_st {
    int sec;
    anytime_handler handler;
    const char* arg;
    int pos;
    // 1: run 0: pause
    int flag;
};

static struct anytime_st* timers[ANYTIME_MAX];

static int find_free_pos(void)
{
    for (int i = 0; i < ANYTIME_MAX; i++)
        if (timers[i] == NULL)
            return i;
    return -1;
}

static void alrm_handler(int s)
{
    alarm(1);
    for (int i = 0; i < ANYTIME_MAX; i++) {
        if (timers[i] != NULL)
            if (--timers[i]->sec == 0)
                timers[i]->handler((void*)timers[i]->arg);
    }
}

static __sighandler_t alrm_handler_befor;

static void module_unload(void)
{
    signal(SIGALRM, alrm_handler_befor);
    alarm(0);
    for (int i = 0; i < ANYTIME_MAX; i++)
        free(timers[i]);
}

void anytime_init(void)
{
    alrm_handler_befor = signal(SIGALRM, alrm_handler);
    alarm(1);
    atexit(module_unload);
}

int anytime_add(int sec, anytime_handler handler, const char* arg)
{
    int pos = find_free_pos();
    if (pos < 0)
        return -ENOSPC;

    struct anytime_st* at;
    at = malloc(sizeof(*at));
    if (at == NULL)
        return -ENOMEM;

    at->sec = sec;
    at->handler = handler;
    at->arg = arg;
    at->pos = pos;

    timers[pos] = at;

    return pos;
}

int anytime_cancel(int jobid)
{
    if (jobid < 0 || jobid >= ANYTIME_MAX)
        return EINVAL;
    free(timers[jobid]);
    timers[jobid] = NULL;
    return 0;
}

void anytime_destroy(void)
{
    for (int i = 0; i < ANYTIME_MAX; i++) {
        if (timers[i] != NULL) {
            free(timers[i]);
            timers[i] = NULL;
        }
    }
}
