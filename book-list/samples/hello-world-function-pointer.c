static int (*printf)(const char *format);

int main(void)
{
    printf = 0;
    printf("Hello, world!\n");
    return 0;
}
