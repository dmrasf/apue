#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__ssize_t mygetline(char**, size_t*, FILE*);
void mygetlinefree(char*);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: <pathname>\n");
        exit(1);
    }

    FILE* fp = NULL;
    char* buf = NULL;
    size_t linesize = 0;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen()");
        exit(1);
    }

    while (mygetline(&buf, &linesize, fp) >= 0) {
        printf("%s\n", buf);
        printf("%ld\n", linesize);
    }

    fclose(fp);
    mygetlinefree(buf);
    exit(0);
}

__ssize_t mygetline(char** linebuf, size_t* linesize, FILE* fp)
{
    if (*linebuf == NULL || *linesize == 0) {
        *linesize = 120;
        *linebuf = malloc(*linesize);
    }

    char c;
    int count = 0;

    while ((c = fgetc(fp)) != '\n') {

        if (c == EOF)
            return -1;

        if (count >= *linesize - 2) {
            *linesize *= 2;
            *linebuf = realloc(*linebuf, *linesize);
        } else {
            linebuf[0][count] = c;
            count++;
        }
    }

    linebuf[0][count+1] = '\n';
    linebuf[0][count+2] = '\0';

    return count;
}

void mygetlinefree(char* buf)
{
    free(buf);
}
