#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void a(void);
static void b(void);
static void c(void);
static void d(void);

static jmp_buf env;

int main(void)
{
    printf("%s() begin\n", __FUNCTION__);
    printf("%s call a() begin\n", __FUNCTION__);
    a();
    printf("%s call a() end\n", __FUNCTION__);
    printf("%s() end\n", __FUNCTION__);
    exit(0);
}

static void a(void)
{
    printf("%s() begin\n", __FUNCTION__);
    printf("%s setjmp\n", __FUNCTION__);
    int ret = 0;
    if ((ret = setjmp(env)) == 0) {
        printf("%s call b() begin\n", __FUNCTION__);
        b();
        printf("%s call b() end\n", __FUNCTION__);
    } else {
        printf("%s jmp out; code = %d\n", __FUNCTION__, ret);
    }
    printf("%s() end\n", __FUNCTION__);
}
static void b(void)
{
    printf("%s() begin\n", __FUNCTION__);
    printf("%s call c() begin\n", __FUNCTION__);
    c();
    printf("%s call c() end\n", __FUNCTION__);
    printf("%s() end\n", __FUNCTION__);
}
static void c(void)
{
    printf("%s() begin\n", __FUNCTION__);
    longjmp(env, 10);
    printf("%s call d() begin\n", __FUNCTION__);
    d();
    printf("%s call d() end\n", __FUNCTION__);
    printf("%s() end\n", __FUNCTION__);
}
static void d(void)
{
    printf("%s() begin\n", __FUNCTION__);
    printf("%s() end\n", __FUNCTION__);
}
