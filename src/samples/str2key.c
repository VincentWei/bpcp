#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZEOF_SIZE_T 8

#if SIZEOF_SIZE_T == 8
// 2^40 + 2^8 + 0xb3 = 1099511628211
#   define FNV_PRIME        ((size_t)0x100000001b3ULL)
#   define FNV_INIT         ((size_t)0xcbf29ce484222325ULL)
#else
// 2^24 + 2^8 + 0x93 = 16777619
#   define FNV_PRIME        ((size_t)0x01000193)
#   define FNV_INIT         ((size_t)0x811c9dc5)
#endif

static size_t str2key (const char* str, size_t length)
{
    const unsigned char* s = (const unsigned char*)str;
    size_t hval = FNV_INIT;

    if (str == NULL)
        return 0;

    if (length == 0)
        length = strlen(str);

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*s && length) {

        /* xor the bottom with the current octet */
        hval ^= (size_t)*s++;
        length--;

        /* multiply by the FNV magic prime */
#ifdef __GNUC__
#   if SIZEOF_SIZE_T == 8
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);
#   else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#   endif
#else
        hval *= FNV_PRIME;
#endif
    }

    /* return our new hash value */
    return hval;
}

static const char *categories[] = {
    "ctype",
    "collate",
    "numeric",
    "monetary",
    "message",
    "time",
    "name",
    "telephone",
    "measurement",
    "paper",
    "address",
    "identification",
};

#define NR_CATEGORIES sizeof(categories)/sizeof(categories[0])

static bool try_with_nr_slots(size_t nr_slots)
{
    const char *slots[nr_slots];

    memset(slots, 0, sizeof(const char *) * nr_slots);

    for (size_t i = 0; i < NR_CATEGORIES; i++) {
        size_t hval = str2key(categories[i], strlen(categories[i]));

        hval %= nr_slots;
        printf("%lu, %s\n", hval, categories[i]);
        slots[hval] = categories[i];
    }

    int n = 0;
    for (size_t i = 0; i < nr_slots; i++) {

        if (slots[i]) {
            char buff[64];
            strcpy(buff, "LC_");
            strcat(buff, slots[i]);

            printf("%s,\n", buff);
        }
        else {
            puts("-1,");
            n++;
        }
    }

    n = nr_slots - n;

    return (n == NR_CATEGORIES);
}

int main(void)
{
    size_t nr_slots = NR_CATEGORIES;

    do {
        if (try_with_nr_slots(nr_slots)) {
            printf("found nr_slots: %lu\n", nr_slots);
            break;
        }

        nr_slots++;
    } while (nr_slots < 100);

    return 0;
}

