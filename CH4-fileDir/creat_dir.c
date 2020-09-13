#include "../apue.h"
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int cnt = 0;

    int path_max = sysconf(_PC_NAME_MAX);
    char* path_name;
    path_name = (char*)malloc((path_max + 1) * sizeof(char));
    for (int i = 0; i <= path_max; i++)
        path_name[i] = 'o';

    while (1) {
        if (mkdir(path_name, DIR_MODE) == -1)
            err_quit("creat dir error, cnt = %d\n", cnt);

        cnt++;

        if (chdir(path_name) == -1)
            err_quit("chdir error, cnt = %d\n", cnt);

        if (cnt == 100)
            break;
    }

    return 0;
}
