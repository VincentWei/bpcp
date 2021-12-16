## 第六讲：解耦代码和数据

- 时间：2021 年 12 月 16 日 18:30
- 地点：微信视频号“考鼎录”直播间
- [立即预约](#/3)

		
## 解耦代码和数据的重要性

1. 简化代码，降低出错几率
1. 对杂乱的代码做结构化处理
1. 提高代码和可维护性

	
## 解耦代码和数据的重要性

1. 将数据从代码中分离出来，是软件工程师必须掌握的技巧，跟编程语言无关。
1. 如果连这一关都过不了，那只能算是一个普通的程序员。

		
## 一个简单的例子

- 尝试实现一个类似 `strerror` 的函数：

```c
#include <string.h>

char *strerror(int errnum);
char *strerror_r(int errnum, char *buf, size_t buflen);

const char *my_error_message(int errcode);
```

	
### 一般程序员的实现

```c
#include <errno.h>

const char *my_error_message(int errcode)
{
    if (errcode == EINVAL) {
        return "Invalid argument";
    }
    else if (errcode == EACCES) {
        return "Permission denied";
    }
    ...

    return "Unkown error code";
}
```

或者，


```c
#include <errno.h>

const char *my_error_message(int errcode)
{
    switch (errcode) {
        case EINVAL:
            return "Invalid argument";
        case EACESS:
            return "Permission denied";
        ...
    }

    return "Unkown error code";
}
```

	
### C标准库的实现

```c
#include <errno.h>

const char * const sys_errlist[] = {
    ...,
    "Invalid argument",     // EINVAL
    ...,
    "Permission denied",    // EACESS
    ...,
};

const char *my_error_message(int errcode)
{
    if (errno < sizeof(sys_errlist)/sizeof(sys_errlist[0])) {
        return sys_errlist[errno];
    }

    return "Bad error code";
}
```

	
### 好处和坏处

1. 好处：
   1. 分开维护数据和代码（可维护性）
   1. 增加新的错误码，不需要修改代码（可维护性）
   1. 简化代码结构（代码质量）
   1. 提高执行效率（代码质量）
1. 坏处：
   1. 不太直观，可读性略下降。

		
## 再来一个的例子

- HVML 解释器 PurC 定义的变体数据结构（内部）：

```c
// structure for variant
struct purc_variant {

    /* variant type */
    unsigned int type:8;

    /* real length for short string and byte sequence.
       use the extra space (long string and byte sequence)
       if the value of this field is 0. */
    unsigned int size:8;

    /* flags */
    unsigned int flags:16;

    /* reference count */
    unsigned int refc;

    /* observer listeners */
    struct list_head        listeners;

    /* value */
    union {
        /* for boolean */
        bool        b;

        /* for number */
        double      d;

        /* for long integer */
        int64_t     i64;

        /* for unsigned long integer */
        uint64_t    u64;

        /* for long double */
        long double ld;

        /* for dynamic and native variant (two pointers)
           for native variant,
           ptr_ptr[0] stores the native entity of it's self, and
           ptr_ptr[1] stores the ops that's bound to the class of
           such entity. */
        void*       ptr_ptr[2];

        /* for long string, long byte sequence, array, object,
           and set (sz_ptr[0] for size, sz_ptr[1] for pointer).
           for atom string, sz_ptr[0] stores the atom. */
        /* for string_static, we also store strlen(sz_ptr[1]) into sz_ptr[0] */

        uintptr_t   sz_ptr[2];

        /* for short string and byte sequence; the real space size of `bytes`
           is `max(sizeof(long double), sizeof(void*) * 2)` */
        uint8_t     bytes[0];
    };
};
```

	
### 最初的实现

- 需求：实现一个函数，返回一个变体的类型字符串，比如对数值，返回 `number`。

```c
static purc_variant_t
type_getter (purc_variant_t root, size_t nr_args, purc_variant_t *argv)
{
    UNUSED_PARAM(root);

    purc_variant_t ret_var = PURC_VARIANT_INVALID;

    if (nr_args == 0)  {
        pcinst_set_error (PURC_ERROR_WRONG_ARGS);
        return PURC_VARIANT_INVALID;
    }

    if (argv[0] == PURC_VARIANT_INVALID)  {
        pcinst_set_error (PURC_ERROR_WRONG_ARGS);
        return PURC_VARIANT_INVALID;
    }

    switch ((int)purc_variant_get_type (argv[0]))  {
        case PURC_VARIANT_TYPE_NULL:
            ret_var = purc_variant_make_string ("null", false);
            break;
        case PURC_VARIANT_TYPE_UNDEFINED:
            ret_var = purc_variant_make_string ("undefined", false);
            break;
        case PURC_VARIANT_TYPE_BOOLEAN:
            ret_var = purc_variant_make_string ("boolean", false);
            break;
        case PURC_VARIANT_TYPE_NUMBER:
            ret_var = purc_variant_make_string ("number", false);
            break;
        case PURC_VARIANT_TYPE_LONGINT:
            ret_var = purc_variant_make_string ("longint", false);
            break;
        case PURC_VARIANT_TYPE_ULONGINT:
            ret_var = purc_variant_make_string ("ulongint", false);
            break;
        case PURC_VARIANT_TYPE_LONGDOUBLE:
            ret_var = purc_variant_make_string ("longdouble", false);
            break;
        case PURC_VARIANT_TYPE_ATOMSTRING:
            ret_var = purc_variant_make_string ("atomstring", false);
            break;
        case PURC_VARIANT_TYPE_STRING:
            ret_var = purc_variant_make_string ("string", false);
            break;
        case PURC_VARIANT_TYPE_BSEQUENCE:
            ret_var = purc_variant_make_string ("bsequence", false);
            break;
        case PURC_VARIANT_TYPE_DYNAMIC:
            ret_var = purc_variant_make_string ("dynamic", false);
            break;
        case PURC_VARIANT_TYPE_NATIVE:
            ret_var = purc_variant_make_string ("native", false);
            break;
        case PURC_VARIANT_TYPE_OBJECT:
            ret_var = purc_variant_make_string ("object", false);
            break;
        case PURC_VARIANT_TYPE_ARRAY:
            ret_var = purc_variant_make_string ("array", false);
            break;
        case PURC_VARIANT_TYPE_SET:
            ret_var = purc_variant_make_string ("set", false);
            break;
        default:
            break;
    }

    return ret_var;

}
```

	
### 改进的版本

1. 从代码中将数据分离了出来。
1. 通过 `assert` 避免出现未有效维护的情形。

```c
typedef enum purc_variant_type
{
    PURC_VARIANT_TYPE_FIRST = 0,

    /* critical: keep order as is */
    PURC_VARIANT_TYPE_UNDEFINED = PURC_VARIANT_TYPE_FIRST,
    PURC_VARIANT_TYPE_NULL,
    PURC_VARIANT_TYPE_BOOLEAN,
    PURC_VARIANT_TYPE_NUMBER,
    PURC_VARIANT_TYPE_LONGINT,
    PURC_VARIANT_TYPE_ULONGINT,
    PURC_VARIANT_TYPE_LONGDOUBLE,
    PURC_VARIANT_TYPE_ATOMSTRING,
    PURC_VARIANT_TYPE_STRING,
    PURC_VARIANT_TYPE_BSEQUENCE,
    PURC_VARIANT_TYPE_DYNAMIC,
    PURC_VARIANT_TYPE_NATIVE,
    PURC_VARIANT_TYPE_OBJECT,
    PURC_VARIANT_TYPE_ARRAY,
    PURC_VARIANT_TYPE_SET,

    /* critical: this MUST be the last enum */
    PURC_VARIANT_TYPE_LAST = PURC_VARIANT_TYPE_SET,
} purc_variant_type;

#define PURC_VARIANT_TYPE_NR \
    (PURC_VARIANT_TYPE_LAST - PURC_VARIANT_TYPE_FIRST + 1)

static purc_variant_t
type_getter (purc_variant_t root, size_t nr_args, purc_variant_t *argv)
{
    static const char *type_names[] = {
        "undefined",
        "null",
        "boolean",
        "number",
        "longint",
        "ulongint",
        "longdouble",
        "atomstring",
        "string",
        "bsequence",
        "dynamic",
        "native",
        "object",
        "array",
        "set",
    };

    /* make sure that the last one is `set` */
    assert (sizeof(type_names[PURC_VARIANT_TYPE_FIRST]) == 10);
    assert (sizeof(type_names[PURC_VARIANT_TYPE_LAST]) == 4);

    UNUSED_PARAM(root);

    if ((argv == NULL) || (nr_args == 0)) {
        pcinst_set_error (PURC_ERROR_WRONG_ARGS);
        return PURC_VARIANT_INVALID;
    }

    assert (argv[0] != PURC_VARIANT_INVALID);

    return purc_variant_make_string_static (
            type_names [purc_variant_get_type (argv[0])], false);
}
```

	
### 终极版本

1. 使用宏定义字符串常量，避免手误产生的错误。
1. 使用编译期断言避免出现未有效维护的情形。

```c
static const char *type_names[] = {
    VARIANT_TYPE_NAME_UNDEFINED,
    VARIANT_TYPE_NAME_NULL,
    VARIANT_TYPE_NAME_BOOLEAN,
    VARIANT_TYPE_NAME_NUMBER,
    VARIANT_TYPE_NAME_LONGINT,
    VARIANT_TYPE_NAME_ULONGINT,
    VARIANT_TYPE_NAME_LONGDOUBLE,
    VARIANT_TYPE_NAME_ATOMSTRING,
    VARIANT_TYPE_NAME_STRING,
    VARIANT_TYPE_NAME_BYTESEQUENCE,
    VARIANT_TYPE_NAME_DYNAMIC,
    VARIANT_TYPE_NAME_NATIVE,
    VARIANT_TYPE_NAME_OBJECT,
    VARIANT_TYPE_NAME_ARRAY,
    VARIANT_TYPE_NAME_SET,
};

/* Make sure the number of variant types matches the size of `type_names` */
#define _COMPILE_TIME_ASSERT(name, x)               \
       typedef int _dummy_ ## name[(x) * 2 - 1]

_COMPILE_TIME_ASSERT(types, PCA_TABLESIZE(type_names) == PURC_VARIANT_TYPE_NR);

#undef _COMPILE_TIME_ASSERT

static purc_variant_t
type_getter (purc_variant_t root, size_t nr_args, purc_variant_t *argv)
{
    UNUSED_PARAM(root);

    if ((argv == NULL) || (nr_args == 0)) {
        pcinst_set_error (PURC_ERROR_WRONG_ARGS);
        return PURC_VARIANT_INVALID;
    }

    assert (argv[0] != PURC_VARIANT_INVALID);

    /* make sure that the first one is `undefined` */
    assert (strcmp (type_names[PURC_VARIANT_TYPE_FIRST],
                VARIANT_TYPE_NAME_UNDEFINED) == 0);
    /* make sure that the last one is `set` */
    assert (strcmp (type_names[PURC_VARIANT_TYPE_LAST],
                VARIANT_TYPE_NAME_SET) == 0);

    return purc_variant_make_string_static (
            type_names [purc_variant_get_type (argv[0])], false);
}
```

	
### 结论

好代码是重构出来的！

		
## 更复杂的情形

- 解析如下格式的消息数据包：

```
type: request
target: session/0
operation: create_plain_window
requestId: d63588ae93e9512832510e6ac300d66f
dataType: ejson
dataLen: <length of data in bytes>

{
    workspace: 3456789,
    name: "_current",
    title: "Hello, world!",
    class: "normal",
}
```

		
## Q & A

