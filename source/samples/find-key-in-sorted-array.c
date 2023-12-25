#include <string.h>
#include <stdio.h>

#include "find-key-in-sorted-array.h"

#define STR_KEY_TYPE        "type"
#define STR_KEY_TARGET      "target"
#define STR_KEY_OPERATION   "operation"
#define STR_KEY_ELEMENT     "element"
#define STR_KEY_PROPERTY    "property"
#define STR_KEY_EVENT       "event"
#define STR_KEY_REQUEST_ID  "requestId"
#define STR_KEY_RESULT      "result"
#define STR_KEY_DATA_TYPE   "dataType"
#define STR_KEY_DATA_LEN    "dataLen"

static struct key_id_pair {
    const char *key;
    int         id;
} key_ids[] = {
    { STR_KEY_DATA_LEN,     1 },
    { STR_KEY_DATA_TYPE,    2 },
    { STR_KEY_ELEMENT,      3 },
    { STR_KEY_EVENT,        4 },
    { STR_KEY_OPERATION,    5 },
    { STR_KEY_PROPERTY,     6 },
    { STR_KEY_REQUEST_ID,   7 },
    { STR_KEY_RESULT,       8 },
    { STR_KEY_TYPE,         9 },
    { STR_KEY_TARGET,       10 },
};

find_key_in_sorted_array(find_key_id, key_ids, const char *, int,
        key, id, strcasecmp, -1);

static struct int_key_id_pair {
    int         key;
    int         id;
} int_key_ids[] = {
    { 2,    1 },
    { 3,    2 },
    { 5,    3 },
    { 8,    4 },
    { 13,   5 },
    { 21,   6 },
    { 34,   7 },
    { 55,   8 },
    { 89,   9 },
};

static inline int intcmp(int i1, int i2)
{
    return i1 - i2;
}

find_key_in_sorted_array(find_int_key_id, int_key_ids, int, int,
        key, id, intcmp, -1);

int main(void)
{
    int id = find_key_id("type");
    printf("Value of key (%s): %d\n", "type", id);

    id = find_int_key_id(55);
    printf("Value of key (%d): %d\n", 55, id);
    return 0;
}

