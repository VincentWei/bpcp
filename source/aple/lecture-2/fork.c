#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd = open("fork.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0)
        return 1;

    pid_t pid = fork();
    if (pid > 0) {
        write(fd, "1234", 4);
    }
    else if (pid == 0) {
        write(fd, "ABCDEFGH", 8);
        close(fd);
        _exit(0);
    }
    else {
        return 1;
    }

    write(fd, "5678", 4);
    close(fd);
    return 0;
}

