/*
 *  ./a.out file.hole r10 s5 w32
 */
#include "../apue.h"
#include "get_num.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    size_t len;
    off_t offset;
    int fd, ap, j;
    char* buf;
    ssize_t numRead, numWrite;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        err_quit("%s file {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);

    fd = open(argv[1], O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (fd == -1)
        err_quit("open");

    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':
        case 'R':
            len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            buf = malloc(len);
            if (buf == NULL)
                err_quit("malloc");

            numRead = read(fd, buf, len);
            if (numRead == -1)
                err_quit("read");

            if (numRead == 0) {
                printf("%s: end-of-file\n", argv[ap]);
            } else {
                printf("%s: ", argv[ap]);
                for (j = 0; j < numRead; j++) {
                    if (argv[ap][0] == 'r') {
                        printf("%c", isprint((unsigned char)buf[j]) ? buf[j] : '?');
                    } else {
                        printf("%02x", (unsigned char)buf[j]);
                    }
                }
                printf("\n");
            }
            free(buf);
            break;

        case 'w':
            numWrite = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (numWrite == -1)
                err_quit("write");
            printf("%s: wrote %ld bytes\n", argv[ap], (long)numWrite);
            break;

        case 's':
            offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            if (lseek(fd, offset, SEEK_SET) == -1)
                err_quit("lseek");
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            err_quit("cmd error");
        }
    }

    exit(0);
}
