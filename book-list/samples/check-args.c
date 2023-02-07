#include <stdlib.h>
#include <string.h>

static int check_argc(int argc);
static int check_args(const char *argv[]);

int main(int argc, const char *argv[])
{
    if (check_argc(argc) || check_args(argv))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

#define MIN_ARGC        1

static int check_argc(int argc)
{
    if (argc < MIN_ARGC)
        return -1;
    return 0;
}

#define MIN_LEN_ARG    5

static int check_args(const char *argv[])
{
    if (strlen(argv[0]) < MIN_LEN_ARG)
        return -1;
    return 0;
}
