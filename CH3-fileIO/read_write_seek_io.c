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

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (fd == -1) {
        fprintf(stderr, "fd\n");
        exit(1);
    }

    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':
        case 'R':
            len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            buf = malloc(len);
            if (buf == NULL) {
                fprintf(stderr, "malloc()\n");
                exit(1);
            }

            numRead = read(fd, buf, len);
            if (numRead == -1) {
                perror("read");
                exit(1);
            }

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
            if (numWrite == -1) {
                perror("write()");
                exit(1);
            }
            printf("%s: wrote %ld bytes\n", argv[ap], (long)numWrite);
            break;

        case 's':
            offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            if (lseek(fd, offset, SEEK_SET) == -1) {
                perror("lseek");
                exit(1);
            }
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            exit(1);
        }
    }

    exit(0);
}
