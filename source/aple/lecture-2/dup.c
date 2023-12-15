#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd1 = open("dup.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd1 < 0)
        return 1;

    int fd2 = dup(fd1);
    if (fd2 < 0)
        return 1;

    write(fd1, "1234", 4);
    write(fd2, "ABCDEFGH", 8);
    write(fd1, "5678", 4);

    close(fd1);
    close(fd2);
    return 0;
}

