
#define find_key_in_sorted_array(func_name, array, key_type, value_type,    \
        key_field, value_field, cmp_func, not_found_value)                  \
    static value_type func_name(key_type key)                               \
    {                                                                       \
        static ssize_t max = sizeof(array)/sizeof(array[0]) - 1;            \
        ssize_t low = 0, high = max, mid;                                   \
        while (low <= high) {                                               \
            int cmp;                                                        \
            mid = (low + high) / 2;                                         \
            cmp = cmp_func(key, array[mid].key_field);                      \
            if (cmp == 0) {                                                 \
                goto found;                                                 \
            }                                                               \
            else {                                                          \
                if (cmp < 0) {                                              \
                    high = mid - 1;                                         \
                }                                                           \
                else {                                                      \
                    low = mid + 1;                                          \
                }                                                           \
            }                                                               \
        }                                                                   \
        return not_found_value;                                             \
    found:                                                                  \
        return array[mid].value_field;                                      \
    }
