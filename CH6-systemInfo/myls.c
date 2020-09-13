#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define NAMESIZE 1024

static char* getmode(struct stat*);
static __nlink_t getnlink(struct stat *);
static char* getgrname(struct stat *);
static char* geturname(struct stat *);

int main(int argc, char* argv[])
{
    FILE* fp;
    struct stat st;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    if ((lstat(argv[1], &st)) == -1) {
        perror("lstat");
        exit(1);
    }

    struct timespec mtim;

    mtim = st.st_mtim;

    printf("%ld\n", mtim.tv_sec);
    printf("%ld\n", mtim.tv_nsec);

    printf("%s %ld %s %s\n",getmode(&st), getnlink(&st), getgrname(&st), geturname(&st));


    fclose(fp);
    exit(0);
}

static char* getmode(struct stat* st)
{
    char* mode = (char*)malloc(11 * sizeof(char));
    mode[10] = '\0';

    switch (st->st_mode & S_IFMT) {
    case S_IFBLK: mode[0]  = 'b'; break;
    case S_IFCHR: mode[0]  = 'c'; break;
    case S_IFDIR: mode[0]  = 'd'; break;
    case S_IFIFO: mode[0]  = 'p'; break;
    case S_IFLNK: mode[0]  = 'l'; break;
    case S_IFREG: mode[0]  = '-'; break;
    case S_IFSOCK: mode[0] = 's'; break;
    default: mode[0]       = '?'; break;
    }

    if (st->st_mode & S_IRUSR) mode[1] = 'r'; else mode[1] = '-';
    if (st->st_mode & S_IWUSR) mode[2] = 'w'; else mode[2] = '-';
    if (st->st_mode & S_IXUSR) mode[3] = 'x'; else mode[3] = '-';
    if (st->st_mode & S_IRGRP) mode[4] = 'r'; else mode[4] = '-';
    if (st->st_mode & S_IWGRP) mode[5] = 'w'; else mode[5] = '-';
    if (st->st_mode & S_IXGRP) mode[6] = 'x'; else mode[6] = '-';
    if (st->st_mode & S_IROTH) mode[7] = 'r'; else mode[7] = '-';
    if (st->st_mode & S_IWOTH) mode[8] = 'w'; else mode[8] = '-';
    if (st->st_mode & S_IXOTH) mode[9] = 'x'; else mode[9] = '-';

    return mode;
}

static __nlink_t getnlink(struct stat* st) {
    return st->st_nlink;
}

static char* getgrname(struct stat* st)
{
    struct group* gr = malloc(sizeof(struct group));
    if ((gr = getgrgid(st->st_gid)) == NULL) {
        perror("getgrgid");
        exit(1);
    }
    return gr->gr_name;
}

static char* geturname(struct stat* st)
{
    struct passwd* pw = malloc(sizeof(struct passwd));
    if ((pw = getpwuid(st->st_uid)) == NULL) {
        perror("getpwuid");
        exit(1);
    }
    return pw->pw_name;
}
