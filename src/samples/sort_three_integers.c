#include <stdio.h>
#include <assert.h>

/* use qsort() if there are more integers */
static void sort_three_integers(int *numbers)
{
    int min, mid, max;
    int min_i = 0, max_i = 0, mid_i;

    min = numbers[0];
    max = numbers[0];

    if (numbers[1] < min) {
        min = numbers[1];
        min_i = 1;
    }
    else {
        max = numbers[1];
        max_i = 1;
    }

    if (numbers[2] < min) {
        min = numbers[2];
        min_i = 2;
    }
    else {
        max = numbers[2];
        max_i = 2;
    }

    assert(min_i != max_i);

    mid_i = 3 - min_i - max_i;
    mid = numbers[mid_i];

    numbers[0] = min;
    numbers[1] = mid;
    numbers[2] = max;
}

int main (void)
{
    int i;
    int numbers[3];

    for (i = 0; i < 3; i++) {
        scanf ("%d", numbers + i);
    }

    printf("Input integers: %d, %d, %d\n",
            numbers[0], numbers[1], numbers[2]);

    sort_three_integers(numbers);

    printf("Sorted integers: %d, %d, %d\n",
            numbers[0], numbers[1], numbers[2]);

    return 0;
}

