#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define XOPEN_SOURCE

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    char* pass;
    pass = getpass("Password: ");

    struct spwd* spd;
    if ((spd = getspnam(argv[1])) == NULL) {
        fprintf(stderr, "getspnam error\n");
        exit(1);
    }

    char* cpass;
    // 编译时要加 -lcrypt
    cpass = crypt(pass, spd->sp_pwdp);
    if (strcmp(cpass, spd->sp_pwdp) == 0)
        puts("right");
    else
        puts("error");

    exit(0);
}
