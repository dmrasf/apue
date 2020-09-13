#ifndef ANYTIME_H__
#define ANYTIME_H__

#define ANYTIME_MAX 1024
typedef void anytime_t;
typedef void (*anytime_handler)(void*);

void anytime_init(void);

int anytime_add(int, anytime_handler, const char*);

int anytime_cancel(int);

void anytime_destroy(void);

#endif
