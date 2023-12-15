/* 派生子进程 */

#include <sys/types.h>
#include <unistd.h>

void err_sys (const char* info)
{
    perror (info);
    exit (1);
}

int main (void)
{
    pid_t pid;

    if ( (pid = fork ()) < 0)
        err_sys ("fork error");
    else if (pid > 0) {
        // this is parent.
    }
    else {
        // this is child.
    }

    exit (0);
}

