#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH "/etc/shadow"
#define KEYPROJ 'a'

#define NAMESIZE 64

struct msg_st {
    long mtype;
    char name[NAMESIZE];
    int math;
    int chinese;
};

#endif
