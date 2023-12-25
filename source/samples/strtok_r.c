#include <stdio.h>
#include <string.h>

static void list_tokens(char *str)
{
    char *str1;
    for (str1 = str; ; str1 = NULL) {
        char *saveptr1;
        char *token = strtok_r(str1, ";", &saveptr1);
        if (token == NULL)
            break;

        printf("token: %s\n", token);

        char *str2;
        for (str2 = token; ; str2 = NULL) {
            char *saveptr2;
            char *subtoken = strtok_r(str2, ":", &saveptr2);
            if (subtoken == NULL)
                break;
            printf(" subtoken: %s\n", subtoken);
        }
    }
}

int main(void)
{
    char foo[] = "a:b:c;A:B:C";
    list_tokens(foo);
    return 0;
}

