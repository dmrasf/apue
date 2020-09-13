#include "../apue.h"

#define BSZ 48

int main()
{
    FILE* fp;
    char buf[BSZ];

    // 将一段内存空间设为某个字符
    memset(buf, 'a', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';
    // fmemopen在缓冲区开始处放置null字节
    if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
        err_sys("fmemopen failed");
    printf("initial buffer contents: %s\n", buf);
    // 写入到stdio库缓冲区
    fprintf(fp, "hello");
    printf("before flush: %s\n", buf);
    // 刷新到buf
    // fp 偏移量 12
    fflush(fp);
    printf("after fflush: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'b', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';
    fprintf(fp, "hello");
    // fseek 引起缓冲区冲洗到buf  偏移量24
    fseek(fp, 0, SEEK_SET);
    printf("after  fseek: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'c', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';
    fprintf(fp, "hello, worlddddd");
    // 为超出stdio缓冲区大小 不加null
    fclose(fp);
    printf("after fclose: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    return (0);
}
