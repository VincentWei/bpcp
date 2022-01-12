#include <stdio.h>

int main(void)
{
    char author[30], lang[30];

    printf("Please input author name:\n");
    fgets(author, sizeof(author), stdin);

    printf("Please input lang:\n");
    fgets(lang, sizeof(lang), stdin);

    printf("The author is %s, and the lang is %s.\n", author, lang);

    return 0;
}

