#include <stdlib.h>

#ifndef MYPIPE_H__
#define MYPIPE_H__

#define   PIPESIZE       1024
#define   MYPIPE_READ    0x00000001UL
#define   MYPIPE_WRITE   0x00000002UL

typedef void mypipe_t;

mypipe_t* MyPipeInit();

int MyPipeRegister(mypipe_t*, int);

int MyPipeUnRegister(mypipe_t*, int);

size_t MyPipeRead(mypipe_t*, void* buf, size_t size);

size_t MyPipeWrite(mypipe_t*, const void* buf, size_t size);

int MyPipeDestroy(mypipe_t*);

#endif
