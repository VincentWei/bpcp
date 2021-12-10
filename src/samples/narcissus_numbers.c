#include <stdio.h>
#include <stdbool.h>

static bool is_narcissus(unsigned candidant, int n)
{
    unsigned int t = candidant;
    unsigned int sum = 0;

    do {
        unsigned int d = t % 10;
        unsigned power = d;

        int j = 1;
        while (j < n) {
            power *= d;
            j++;
        }

        sum += power;
        t /= 10;
    } while (t > 0);

    if (sum == candidant) {
        return true;
    }

    return false;
}

int main(void)
{
    int n;

    scanf("%d", &n);
    if (n < 3) {
        printf("N (%d) must be greater than 2!\n", n);
        return 1;
    }

    unsigned int first = 1;
    for (int i = 1; i < n; i++) {
        first *= 10;
    }

    /* just try a known narcissus to check the implementation */
    if (!is_narcissus(153, 3)) {
        printf("Bad implementation!\n");
        return 1;
    }

    printf("We start from %u...\n", first);

    unsigned int found = 0;
    unsigned int candidant;
    for (candidant = first; candidant < first * 10; candidant++) {
        if (is_narcissus(candidant, n)) {
            printf("Found one: %u\n", candidant);
            found++;
        }
    }
    printf("Totally %u found\n", found);

    return 0;
}
