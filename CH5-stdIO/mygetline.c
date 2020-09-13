#include <stdio.h>
#include <stdlib.h>

__ssize_t mygetline(char**, size_t*, FILE*);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: <pathname>\n");
        exit(1);
    }

    FILE* fp = NULL;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen() ");
        exit(1);
    }

    char c;
    int count = 0;

    while ((c = fgetc(fp)) != '\n') {
        fputc(c, stdout);
        count++;
    }
    printf("\n%d\n", count);


    char *linebuf;
    size_t linesize;

    int size = mygetline(&linebuf, &linesize, fp);

    printf("%s\n", linebuf);
    printf("%d\n", size);
    printf("%ld\n", linesize);


    fclose(fp);
    return 0;
}

__ssize_t mygetline(char** linebuf, size_t* linesize, FILE* fp)
{
    *linesize = 120;
    linebuf[0] = (char*)malloc(*linesize);

    char c;
    int count = 0;

    while (1) {
        c = fgetc(fp);
        if (c == EOF) {
            return -1;
        } else if (c == '\n') {
            break;
        } else {
            linebuf[0][count] = c;
            count++;
            if (count >= *linesize) {
                linebuf[0] = realloc(linebuf[0], *linesize);
                *linesize *= 2;
            }
        }
    }

    return count;
}
