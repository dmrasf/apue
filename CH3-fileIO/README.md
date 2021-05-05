## 第三章 文件I/O


* [3.1 引言](#3.1-引言)
* [3.2 文件描述符号](#3.2-文件描述符号)
* [3.3 函数`open`和`openat`](#3.3-函数open和openat)
* [3.4 函数`creat`](#3.4-函数creat)
* [3.5 函数`close`](#3.5-函数close)
* [3.6 函数`lseek`](#3.6-函数lseek)
* [3.7 函数`read`](#3.7-函数read)
* [3.8 函数`write`](#3.8-函数write)
* [3.10 文件共享](#3.10-文件共享)
* [3.11 原子操作](#3.11-原子操作)
    * [1. 追加到一个文件](#1.-追加到一个文件)
    * [2. 函数`pread`和`pwrite`](#2.-函数pread和pwrite)
* [3.12 函数`dup`和`dup2`](#3.12-函数dup和dup2)
* [3.13 函数`sync`、`fsync`和`fdatasync`](#3.13-函数syncfsync和fdatasync)
* [3.14 函数`fcntl`](#3.14-函数fcntl)
* [3.15 函数`ioctl`](#3.15-函数ioctl)
* [习题](#习题)


#### 3.1 引言

**主要函数`open`, `read`, `write`, `lseek`, `close`**

#### 3.2 文件描述符号

- **数组下标 ➜  文件描述符号** 所以文件描述符数值为非负整数
- **特殊** `STDIN_FILENO: 0; STDOUT_FILENO: 1; STDERR_FILENO: 2;`

#### 3.3 函数`open`和`openat`

```c
#include <fcntl.h>

// 成功返回文件描述符号，失败返回-1
int open(const char *path, int oflag, ...);
int openat(int fd, const char *path, int oflag, ...);
```
*必须指定，多个用或*
| oflag      | 含义               |
| :-------:  | :--------:         |
| `O_RDONLY` | 只读打开           |
| `O_WRONLY` | 只写打开           |
| `O_RDWR`   | 读写打开           |
| `O_EXEC`   | 只执行打开         |
| `O_SEARCH` | 只搜索打开（目录） |
| `O_TRUNC`  | 如果存在，截断为0  |

**`open`与`openat`区别**

1. 当`path`参数为**绝对**路径名，`fd`参数忽略，`openat` = `open`。
2. 当`path`参数为**相对**路径名，`fd`参数通过打开相对路径名所在的目录获取。

```c
#include <sys/types.h>
#include <dirent.h>

DIR *dir = opendir(const char *name);
// 此处fd为openat参数
int fd = dirfd(dir);
```

#### 3.4 函数`creat`

```c
#include <fcntl.h>

int create(const char *path, mode_t mode);
// 相当于
int open(const char *path, O_WRONLY | O_CREATE | O_TRUNC, mode);
```

#### 3.5 函数`close`

**打开文件后要记住关闭**

不过，当一个进程终止时，内核会关闭它所有的打开文件。

```c
#include <fcntl.h>

int close(int fd);
```

#### 3.6 函数`lseek`

- 每个打开文件都有一个与其相关联的文件偏移量
- 通常是一个非负整数，用以度量从文件开始处计算的字节数
- 读、写操作都从当前文件偏移量处开始，并使偏移量增加所读写的字节数
- 按系统默认的情况，当打开一个文件时，除非指定`O_APPEND`选项，否则该偏移量被设置为0

```c
#include <unistd.h>

// 成功返回新的偏移量
off_t lseek(int fd, off_t offset, int whence);
```
**offset**
- 以字节为单位，可以大于文件当前长度，形成空洞

**whencw**
- `SEEK_SET`: 则将该文件的偏移量设置为距文件开始处offset个字节。
- `SEEK_CUR`，则将该文件的偏移量设置为其当前值加offset，offset可正可负。
- `SEEK_END`，则将该文件的偏移量设置为文件长度加offset，offset可正可负。

**注意：函数返回值有可能为负数，所以判断错误时应该判断是否为-1**

#### 3.7 函数`read`

```c
#include <unistd.h>

// read成功，则返回读到的字节数
// 已到达文件的尾端，则返回0
// void * 通用指针
size_t read(int fd, void *buf size_t nbytes);
```

#### 3.8 函数`write`

```c
#include <unistd.h>

// 返回写入的字节数，应该与nbytes的值相同
size_t write(int fd, void *buf size_t nbytes);
```

在一次成功后，文件偏移量增加实际写的字节数

#### 3.10 文件共享

![filestruct](./filestruct.png)

**表示打开文件的数据结构** 

1. 每个进程在进程表中都有一个记录项，记录项中包含一张打开文件描述符表，可将其视为一个矢量，每个描述符占用一项。与每个文件描述符相关联的是：
    - 文件描述符标志（close_on_exec，参见图3-7和3.14节）；
    - 指向一个文件表项的指针。

2. 内核为所有打开文件维持一张文件表。每个文件表项包含：
    - 文件状态标志（读、写、添写、同步和非阻塞等，关于这些标志的更多信息参见3.14节）；
    - 当前文件偏移量；
    - 指向该文件v节点表项的指针。

3. 内存中每个文件（或设备）都有一个 v 节点（v-node）结构。v 节点包含了文件类型和对此文件进行各种操作函数的指针。对于大多数文件，v节点还包含了该文件的i节点（i-node，索引节点）。这些信息是在打开文件时从磁盘上读入内存的，所以，文件的所有相关信息都是随时可用的。

#### 3.11 原子操作

##### 1. 追加到一个文件

打开文件时设置`O_APPEND`标志。使得内核在每次写操作之前，
都将进程的当前偏移量设置到该文件的尾端处，于是在每次写之前就不再需要调用lseek。

##### 2. 函数`pread`和`pwrite`

**相当于调用lseek后再调用**

```c
#include <unistd.h>

// 返回值：读到的字节数，若已到文件尾，返回0；若出错，返回−1
ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset);
// 返回值：若成功，返回已写的字节数；若出错，返回−1
ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);
```

**区别**
- 调用时，无法中断其定位和读操作。
- 不更新当前文件偏移量。

#### 3.12 函数`dup`和`dup2`

**两个函数都可用来复制一个现有的文件描述符。**

```c
#include <unistd.h>

int dup(int fd);
int dup2(int fd, int fd2);
```
![dup](./dup.png)

- 由dup返回的新文件描述符一定是当前可用文件描述符中的最小数值。
- 对于 dup2，可以用fd2参数指定新描述符的值。如果fd2已经打开，则先将其关闭。
- 如若fd等于fd2，则dup2返回fd2，而不关闭它。
- 否则，fd2的`FD_CLOEXEC`文件描述符标志就被清除，这样fd2在进程调用exec时是打开状态。

[测试例子](./test_dup.c)

#### 3.13 函数`sync`、`fsync`和`fdatasync`

**同步内核缓冲区到磁盘**

```c
#include <unistd.h>

// 通常，称为update的系统守护进程周期性地调用（一般每隔30秒）sync函数。
// 这就保证了定期冲洗（flush）内核的块缓冲区。
void sync(void);
// 成功 0  错误 -1
int fsync(int fd);
int fdatasync(int fd);
```

* 详情见 [第五章 标准I/O库](../CH5-stdIO)

#### 3.14 函数`fcntl`

**改变已经打开文件的属性，很强大**

```c
#include <fcntl.h>

// 成功返回值与cmd有关  错误返回 -1
int fcntl(int fd, int cmd, ...);
```

**fcntl函数有以下5种功能：**
1. 复制一个已有的描述符（cmd=`F_DUPFD`或`F_DUPFD_CLOEXEC`）。
2. 获取/设置文件描述符标志（cmd=`F_GETFD`或`F_SETFD`）。
3. 获取/设置文件状态标志（cmd=`F_GETFL`或`F_SETFL`）。
4. 获取/设置异步I/O所有权（cmd=`F_GETOWN`或`F_SETOWN`）。
5. 获取/设置记录锁（cmd=`F_GETLK`、`F_SETLK`或`F_SETLKW`）。

**F_DUPFD** 
- 复制文件描述符fd。新文件描述符作为函数值返回。
它是尚未打开的各描述符中大于或等于第3个参数值（取为整型值）中各值的最小值。
新描述符与 fd共享同一文件表项。但是，新描述符有它自己的一套文件描述符标志。

**F_DUPFD_CLOEXEC**
- 复制文件描述符，设置与新描述符关联的FD_CLOEXEC文件描述符标志的值，返回新文件描述符。

**F_GETFD**
- 对应于fd的文件描述符标志作为函数值返回。当前只定义了一个文件描述符标志FD_CLOEXEC。

**F_SETFD**
- 对于fd设置文件描述符标志。新标志值按第3个参数（取为整型值）设置。

**F_GETFL**
- 对应于fd的文件状态标志作为函数值返回。

    *遗憾的是，5个访问方式标志（O_RDONLY、O_WRONLY、O_RDWR、O_EXEC以及O_SEARCH）
    并不各占1位（如前所述，由于历史原因，前3个标志的值分别是0、1和2。
    这5个值互斥，一个文件的访问方式只能取这5个值之一）。
    因此首先必须用屏蔽字O_ACCMODE取得访问方式位，然后将结果与这5个值中的每一个相比较。*

**F_SETFL**
- 将文件状态标志设置为第3个参数的值（取为整型值）。可以更改的几个标志是：O_APPEND、O_NONBLOCK、O_SYNC、O_DSYNC、O_RSYNC、O_FSYNC和O_ASYNC。

**F_GETOWN**
- 获取当前接收SIGIO和SIGURG信号的进程ID或进程组ID。14.5.2节将论述这两种异步I/O信号。

**F_SETOWN**
- 设置接收SIGIO和SIGURG信号的进程ID或进程组ID。正的arg指定一个进程ID，负的arg表示等于arg绝对值的一个进程组ID。

    *fcntl的返回值与命令有关。如果出错，所有命令都返回－1，如果成功则返回某个其他值。
    下列4个命令有特定返回值：F_DUPFD、F_GETFD、F_GETFL以及F_GETOWN。
    第1个命令返回新的文件描述符，第2个和第3个命令返回相应的标志，最后一个命令返回一个正的进程ID或负的进程组ID。*

#### 3.15 函数`ioctl`

```c
#include <sys/ioctl.h>

int ioctl(int fd, int request, ...);
```

#### 习题

##### 3.1 当读/写磁盘文件时，本章中描述的函数确实是不带缓冲机制的吗？请说明原因。

所有磁盘 I/O 都要经过内核的块缓冲区（也称为内核的缓冲区高速缓存）。唯一例外的是对原始磁盘设备的 I/O ，但是我们不考虑这种情况。既然 read 或 write 的数据都要被内核缓冲，那么术语“不带缓冲的 I/O ”指的是在用户的进程中对这两个函数不会自动缓冲，每次 read 或 write 就要进行一次系统调用。

##### 3.2 编写一个与3.12节中`dup2`功能相同的函数，要求不调用`fcntl`函数，并且要有正确的出错处理。

[code](./mydup2.c)

##### 3.3 假设一个进程执行下面3个函数调用：
```c
fd1 = open(path, oflags);
fd2 = dup(fd1);
fd3 = open(path, oflags);
```
画出类似于图3-9的结果图。对fcntl作用于fd1来说，F_SETFD命令会影响哪一个文件描述符？F_SETFL呢？

##### 3.4 许多程序中都包含下面一段代码：
```c
dup2(fd, 0);
dup2(fd, 1);
dup2(fd, 2);
if (fd > 2)
    close(fd);
```
*为了说明if语句的必要性，假设fd是3，画出每次调用dup3时3个描述符项及相应的文件表项的变化情况。然后再画出fd为3的情况。*

fd小于3，执行完3个dup2函数后，不需要关闭；fd大于2，fd没用，需要关闭。

##### 3.5 在Bourne shell、Bourne-again shell和Korn shell中，`digit1>&digit2`表示要将描述符`digit1`重定向至描述符`digit2`的同一文件。请说明下面两条命令的区别。
```sh
./a.out > outfile 2>&1
./a.out 2>&1 > outfile
```

第一条，`stdout`重定向到文件，`stderr`重定向到`stdout`。
第二条，`stderr`重定向到`stdout`，`stdout`重定向到文件。

##### 3.6 如果使用追加标志打开一个文件以便读、写，能否仍用`lseek`在任一位置开始读？能否用`lseek`更新文件中任一部分的数据？请编写一段程序验证。

写只能在末尾，读可以任意读。[code](./test_append_lseek.c)
