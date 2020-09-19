#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH "/etc/shadow"
#define KEYPROJ 'a'

#define PATHSIZE 1024
#define DATASIZE 1024

#define PATHTYPE 1
#define DATATYPE 2
#define EOTTYPE 3

typedef struct msg_path_st {
    long mtype;
    char path[PATHSIZE];
} msg_path_t;

typedef struct msg_data_st {
    long mtype;
    char data[DATASIZE];
    int datalen;
} msg_data_t;

typedef struct msg_eot_st {
    long mtype;
} msg_eot_t;

union msg_un {
    long mtype;
    msg_eot_t eotmsg;
    msg_path_t pathmsg;
    msg_data_t datamsg;
};

#endif
