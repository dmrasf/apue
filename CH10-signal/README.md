# 第十章 信号

[toc]

#### 10.2 信号概念

**信号是软件中断**

#### 10.3 函数`signal`

信号会打断阻塞的系统调用：`open`、`write`...

#### 10.4 不可靠的信号

多次相同的信号，会覆盖调用函数的现场

#### 10.5 中断系统的调用

#### 10.6 可重入函数

#### 10.7 `SIGCLD`语义

#### 10.8 可靠信号术语和语义

#### 10.9 函数`kill`和`raise`

```c
#include <signal.h>

int kill(pid_t pid, int signo);
// int kill(getpid(), int signo);
int raise(int signo);
```

- pid > 0: sent to the process with the ID specified by pid
- pid == 0: sent to every process in the process group of the calling process
- pid == -1: sent to every process for which the calling process has permission to dend signals, except for process 1(init)
- pid < -1: sent to every process in the process group whose ID is -pid

#### 10.10 函数`alarm`和`pause`

**设置定时器，超时时产生`SIGALRM`信号，默认动作：终止调用该`alarm`函数的进程**

```c
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
```

**pause函数使调用进程挂起直至捕捉到一个信号**

```c
#include <unistd.h>

int pause(void);
```

#### 10.11 信号集

**表示多个信号的数据类型**

```c
#include <signal.h>

int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);
```

#### 10.12 函数`sigprocmask`

**设置`mask`值**

```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
```

#### 10.13 函数`sigpending`

**获取当前信号发生的信号**

```c
#include <signal.h>

int sigpending(sigset_t *set);
```

#### 10.14 函数`sigaction`

**`signal()`容易发生信号重入：执行信号响应函数是出现其他信号**

#### 10.15 函数`sigsetjmp`和`siglongjmp`

**不能从信号处理函数轻易往外跳，mask尚未恢复。**

#### 10.16 函数`sigsuspend`

**恢复之前状态，并暂停等待**

#### 10.17 函数`abort`

**给当前进程发送`SIGABRT`信号**

#### 10.18 函数`system`

**执行`shell`命令**

**前提（在有信号参与的程序中）**
1. `SIGCHID` 阻塞
2. `SIGINT`、`SIGQUIT` 忽略

#### 10.19 函数`sleep`、`nanosleep`和`clock_nanosleep`

可以用`nanosleep`和`usleep`替代`sleep`

#### 10.20 函数`sigqueue`

#### 10.21 作业控制信号

#### 10.22 信号名和编号


# 补充

#### `sig_atomic_t`类型

原子操作：一条机器指令执行。

#### `volaile`关键字

避免优化时出错：取值时到地址取，而非缓存。

#### 函数`setitimer`

**`slowcat`例子**

```c
#include <sys/time.h>

int getitimer(int which, struct itimerval *value);
int setitimer(int which, const struct itimerval *restrict value,
```

#### `mask`和`pending`

*对每个进程都有*

**mask**
> 信号屏蔽字 默认为1

**pending**
> 记录信号的位图 默认为0

在响应函数时，相同信号再次产生不会有作用(mask)

#### 实时信号



