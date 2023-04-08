
enum method_id {
    DVOBJ_METHOD_first = 0,
    DVOBJ_METHOD_head = DVOBJ_METHOD_first,
    DVOBJ_METHOD_body,
    DVOBJ_METHOD_tail,

    DVOBJ_METHOD_last = DVOBJ_METHOD_tail,
};

#define DVOBJ_METHOD_MAX    DVOBJ_METHOD_last
#define DVOBJ_METHOD_MIN    DVOBJ_METHOD_first
#define DVOBJ_METHOD_NR     (DVOBJ_METHOD_MAX - DVOBJ_METHOD_MIN + 1)

#define METHOD_ID(name)     DVOBJ_METHOD_##name
#define METHOD_STR(name)    #name

static struct method_id_2_name {
    int         id;
    const char* name;
} method_id_2_name_map[] = {
    { METHOD_ID(head), METHOD_STR(head) },
    { METHOD_ID(tail), METHOD_STR(tail) },
};

/* 使用编译时断言确保 method_id_2_name_map 覆盖所有的 method_t 枚举值 */
#define _COMPILE_TIME_ASSERT(name, x)                               \
       typedef int _dummy_ ## name[(x) * 2 - 1]

#define _TABLESIZE(x)    (sizeof(x)/sizeof(x[0]))

_COMPILE_TIME_ASSERT(methods, _TABLESIZE(method_id_2_name_map) ==    \
        DVOBJ_METHOD_NR);

#undef _TABLESIZE
#undef _COMPILE_TIME_ASSERT
