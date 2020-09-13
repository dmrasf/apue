/*
 * Our own header, to be included before all standard system headers.
 */
#ifndef _APUE_H
#define _APUE_H

#define _POSIX_C_SOURCE 200809L

#if defined(SOLARIS) /* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif

#include <sys/stat.h>
#include <sys/termios.h> /* for winsize */
#include <sys/types.h>   /* some systems still require this */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <errno.h>
#include <signal.h> /* for SIG_ERR */
#include <stdarg.h>
#include <stddef.h> /* for offsetof */
#include <stdio.h>  /* for convenience */
#include <stdlib.h> /* for convenience */
#include <string.h> /* for convenience */
#include <unistd.h> /* for convenience */

#define MAXLINE 4096 /* max line length */

/*
 * Default file access permissions for new files.
 */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Default permissions for new directories.
 */
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

typedef void Sigfunc(int); /* for signal handlers */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/*
 * Prototypes for our own functions.
 */
char* path_alloc(size_t*); /* {Prog pathalloc} */
long open_max(void);       /* {Prog openmax} */

int set_cloexec(int); /* {Prog setfd} */
void clr_fl(int, int);
void set_fl(int, int); /* {Prog setfl} */

void pr_exit(int); /* {Prog prexit} */

void pr_mask(const char*);           /* {Prog prmask} */
Sigfunc* signal_intr(int, Sigfunc*); /* {Prog signal_intr_function} */

void daemonize(const char*); /* {Prog daemoninit} */

void sleep_us(unsigned int);              /* {Ex sleepus} */
ssize_t readn(int, void*, size_t);        /* {Prog readn_writen} */
ssize_t writen(int, const void*, size_t); /* {Prog readn_writen} */

int fd_pipe(int*);                                           /* {Prog sock_fdpipe} */
int recv_fd(int, ssize_t (*func)(int, const void*, size_t)); /* {Prog recvfd_sockets} */
int send_fd(int, int);                                       /* {Prog sendfd_sockets} */
int send_err(int, int,
    const char*);                              /* {Prog senderr} */
int serv_listen(const char*);                  /* {Prog servlisten_sockets} */
int serv_accept(int, uid_t*);                  /* {Prog servaccept_sockets} */
int cli_conn(const char*);                     /* {Prog cliconn_sockets} */
int buf_args(char*, int (*func)(int, char**)); /* {Prog bufargs} */

int tty_cbreak(int);               /* {Prog raw} */
int tty_raw(int);                  /* {Prog raw} */
int tty_reset(int);                /* {Prog raw} */
void tty_atexit(void);             /* {Prog raw} */
struct termios* tty_termios(void); /* {Prog raw} */

int ptym_open(char*, int); /* {Prog ptyopen} */
int ptys_open(char*);      /* {Prog ptyopen} */
#ifdef TIOCGWINSZ
pid_t pty_fork(int*, char*, int, const struct termios*,
    const struct winsize*); /* {Prog ptyfork} */
#endif

int lock_reg(int, int, int, off_t, int, off_t); /* {Prog lockreg} */

#define read_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

pid_t lock_test(int, int, off_t, int, off_t); /* {Prog locktest} */

#define is_read_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

void err_msg(const char*, ...); /* {App misc_source} */
void err_dump(const char*, ...) __attribute__((noreturn));
void err_quit(const char*, ...) __attribute__((noreturn));
void err_cont(int, const char*, ...);
void err_exit(int, const char*, ...) __attribute__((noreturn));
void err_ret(const char*, ...);
void err_sys(const char*, ...) __attribute__((noreturn));

void log_msg(const char*, ...); /* {App misc_source} */
void log_open(const char*, int, int);
void log_quit(const char*, ...) __attribute__((noreturn));
void log_ret(const char*, ...);
void log_sys(const char*, ...) __attribute__((noreturn));
void log_exit(int, const char*, ...) __attribute__((noreturn));

void TELL_WAIT(void); /* parent/child from {Sec race_conditions} */
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);

static void err_doit(int, int, const char*, va_list);

/*  
 *  * Nonfatal error related to a system call.  
 *  * Print a message and return.  
 *  */
void err_ret(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
}

/*  
 *  * Fatal error related to a system call.  
 *  * Print a message and terminate.  
 *  */
void err_sys(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}

/*  
 *  * Fatal error unrelated to a system call.  
 *  * Error code passed as explict parameter.  
 *  * Print a message and terminate.  
 *  */
void err_exit(int error, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, error, fmt, ap);
    va_end(ap);
    exit(1);
}

/*  
 *  * Fatal error related to a system call.  
 *  * Print a message, dump core, and terminate.  
 *  */
void err_dump(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    abort(); /* dump core and terminate */
    exit(1); /* shouldn't get here */
}

/*  
 *  * Nonfatal error unrelated to a system call.  
 *  * Print a message and return.  
 *  */
void err_msg(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}

/*  
 *  * Fatal error unrelated to a system call.  
 *  * Print a message and terminate.  
 *  */
void err_quit(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

/*  
 *  * Print a message and return to caller.  
 *  * Caller specifies "errnoflag".  
 *  */
static void
err_doit(int errnoflag, int error, const char* fmt, va_list ap)
{
    char buf[MAXLINE];
    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag)
        snprintf(buf + strlen(buf), MAXLINE - strlen(buf), ": %s", strerror(error));
    strcat(buf, " ");
    fflush(stdout); /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(NULL); /* flushes all stdio output streams */
}

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

char* path_alloc(size_t* sizep) /* also return allocated size, if nonnull */
{
    char* ptr;
    size_t size;

    if (posix_version == 0)
        posix_version = sysconf(_SC_VERSION);

    if (xsi_version == 0)
        xsi_version = sysconf(_SC_XOPEN_VERSION);

    if (pathmax == 0) { /* first time through */
        errno = 0;
        if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
            if (errno == 0)
                pathmax = PATH_MAX_GUESS; /* it's indeterminate */
            else
                err_sys("pathconf error for _PC_PATH_MAX");
        } else {
            pathmax++; /* add one since it's relative to root */
        }
    }

    /*
	 * Before POSIX.1-2001, we aren't guaranteed that PATH_MAX includes
	 * the terminating null byte.  Same goes for XPG3.
	 */
    if ((posix_version < 200112L) && (xsi_version < 4))
        size = pathmax + 1;
    else
        size = pathmax;

    if ((ptr = malloc(size)) == NULL)
        err_sys("malloc error for pathname");

    if (sizep != NULL)
        *sizep = size;
    return (ptr);
}

#endif /* _APUE_H */
