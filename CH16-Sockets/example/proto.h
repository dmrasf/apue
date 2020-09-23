#ifndef PROTO_H__
#define PROTO_H__

#include <bits/stdint-uintn.h>
#include <stdint.h>

#define RECPORT "3333"

#define NAMEMAX 512 - 8 - 8

struct msg_st {
    uint32_t math;
    uint32_t chinese;
    uint8_t name[1];
} __attribute__((packed));

#endif
