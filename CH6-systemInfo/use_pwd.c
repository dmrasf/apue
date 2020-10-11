#include "../apue.h"
#include <pwd.h>
#include <stdio.h>
#include <string.h>

static struct passwd* my_getpwnam(const char* name)
{
    struct passwd* ptr;

    setpwent();
    while ((ptr = getpwent()) != NULL)
        if (strcmp(name, ptr->pw_name) == 0)
            break;
    endpwent();
    return ptr;
}

int main(void)
{
    struct passwd* pwd;

    pwd = my_getpwnam("dmr");

    printf("%s\n", pwd->pw_dir);
    printf("%s\n", pwd->pw_gecos);
    printf("%d\n", pwd->pw_gid);
    printf("%s\n", pwd->pw_name);
    printf("%s\n", pwd->pw_shell);
    printf("%d\n", pwd->pw_uid);
    printf("%s\n", pwd->pw_passwd);

    return 0;
}
