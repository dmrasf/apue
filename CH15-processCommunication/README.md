# 第十五章 进程间通信

[toc]

#### 15.1 引言

#### 15.2 管道

单工(队列)  自同步机制

匿名管道

pipe()

命名管道

mkfifo()

#### 15.3 函数`popen`和`pclose`

#### 15.4 协同进程

#### 15.5 FIFO

#### 15.6 XSI IPC

IPC: Inter-process communication

消息队列(Message Queues)

信号量(Semaphore Arrays)

共享存储器(Shared Memory)

##### 15.6.1 标识符和键

**每个内核中的 IPC 结构（消息队列、信号量或共享存储段）都用一个非负整数的标识符（identifier）加以引用**

*当一个IPC结构被创建，然后又被删除时，与这种结构相关的标识符连续加1，直至达到一个整型数的最大正值，然后又回转到0。*

##### 15.6.2 权限结构

##### 15.6.3 结构限制

##### 15.6.4 优点和缺点

#### 15.7 消息队列

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgget(key_t key, int msgflg);

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```


#### 15.8 信号量

#### 15.9 共享内存

#### 15.10 POSIX 信号量

#### 15.11 客户进程-服务器进程属性

