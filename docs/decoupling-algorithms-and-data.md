## 第七章：解耦代码和数据

1. 解耦代码和数据的重要性
1. 一个简单的例子
1. 充分利用编译时和运行时断言宏
1. 利用回调函数解耦数据和代码
1. 自动生成代码

		
## 解耦代码和数据的重要性

1. 简化代码，降低出错几率
1. 对杂乱的代码做结构化处理
1. 提高代码的可维护性

	
满是漏洞的字符串对比代码：

```c
    if (kwlen == 4 && strncasecmp(keyword, "read", kwlen) == 0) {
        /* 处理关键词 read */
        ...
    }
    else if (kwlen == 4 && strncasecmp(keyword, "write", kwlen) == 0) {
        /* 处理关键词 write */
        ...
    }
```

	
有效管理常量的版本：


```c
#define strncasecmp2ltr(str, literal, len)      \
    ((len > (sizeof(literal "") - 1)) ? 1 :     \
        (len < (sizeof(literal "") - 1) ? -1 : strncasecmp(str, literal, len)))

#define KEYWORD_READ        "read"
#define KEYWORD_WRITE       "write"

...

    if (strncasecmp2ltr(keyword, KEYWORD_READ) == 0) {
        ...
    }
    else if (strncasecmp2ltr(keyword, KEYWORD_WRITE) == 0) {
        ...
    }
```

	
仍有问题：

- 如果增加了新的关键词匹配该怎么办？
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

	
### 或者换个写法

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

	
### 最佳实现

```c
#include <errno.h>

#define MAX_ERRCODE                    ENOTSUP

const char * const sys_errlist[] = {
    ...,
    "Invalid argument",             /* EINVAL */
    ...,
    "Permission denied",            /* EACESS */
    ...,
    "Operation not supported",      /* ENOTSUP 95 */
};

#define _COMPILE_TIME_ASSERT(name, x)               \
       typedef int _dummy_ ## name[(x) * 2 - 1]

/* 确保 my_errlist 中包含有 MAX_ERRCODE 个字符串常量。 */
_COMPILE_TIME_ASSERT(types, sizeof(my_errlist)/sizeof(my_errlist[0]) == MAX_ERRCODE);

#undef _COMPILE_TIME_ASSERT
const char *my_error_message(int errcode)
{
    /* 确保 my_errlist 中包含有 MAX_ERRCODE 个字符串常量。 */
    assert(sizeof(my_errlist)/sizeof(my_errlist[0]) == MAX_ERRCODE);

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

		
## 再来一个例子：充分利用断言宏

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

- 实现一个函数，返回一个变体的类型字符串，比如对数值类型返回 `"number"`。

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
typedef enum purc_variant_type {
    PURC_VARIANT_TYPE_FIRST = 0,

    /* XXX：注意类型枚举值和类型名称要匹配。 */
#define PURC_VARIANT_TYPE_NAME_UNDEFINED    "undefined"
    PURC_VARIANT_TYPE_UNDEFINED = PURC_VARIANT_TYPE_FIRST,
#define PURC_VARIANT_TYPE_NAME_FIRST        PURC_VARIANT_TYPE_NAME_UNDEFINED
#define PURC_VARIANT_TYPE_NAME_NULL         "null"
    PURC_VARIANT_TYPE_NULL,
#define PURC_VARIANT_TYPE_NAME_BOOLEAN      "boolean"
    PURC_VARIANT_TYPE_BOOLEAN,
#define PURC_VARIANT_TYPE_NAME_EXCEPTION    "exception"
    PURC_VARIANT_TYPE_EXCEPTION,
#define PURC_VARIANT_TYPE_NAME_NUMBER       "number"
    PURC_VARIANT_TYPE_NUMBER,
#define PURC_VARIANT_TYPE_NAME_LONGINT      "longint"
    PURC_VARIANT_TYPE_LONGINT,
#define PURC_VARIANT_TYPE_NAME_ULONGINT     "ulongint"
    PURC_VARIANT_TYPE_ULONGINT,
#define PURC_VARIANT_TYPE_NAME_LONGDOUBLE   "longdouble"
    PURC_VARIANT_TYPE_LONGDOUBLE,
#define PURC_VARIANT_TYPE_NAME_ATOMSTRING   "atomstring"
    PURC_VARIANT_TYPE_ATOMSTRING,
#define PURC_VARIANT_TYPE_NAME_STRING       "string"
    PURC_VARIANT_TYPE_STRING,
#define PURC_VARIANT_TYPE_NAME_BYTESEQUENCE "bsequence"
    PURC_VARIANT_TYPE_BSEQUENCE,
#define PURC_VARIANT_TYPE_NAME_DYNAMIC      "dynamic"
    PURC_VARIANT_TYPE_DYNAMIC,
#define PURC_VARIANT_TYPE_NAME_NATIVE       "native"
    PURC_VARIANT_TYPE_NATIVE,
#define PURC_VARIANT_TYPE_NAME_OBJECT       "object"
    PURC_VARIANT_TYPE_OBJECT,
#define PURC_VARIANT_TYPE_NAME_ARRAY        "array"
    PURC_VARIANT_TYPE_ARRAY,
#define PURC_VARIANT_TYPE_NAME_SET          "set"
    PURC_VARIANT_TYPE_SET,
#define PURC_VARIANT_TYPE_NAME_TUPLE        "tuple"
    PURC_VARIANT_TYPE_TUPLE,

    /* XXX：新增类型时，注意修改下面的宏定义。 */
    PURC_VARIANT_TYPE_LAST = PURC_VARIANT_TYPE_TUPLE,
#define PURC_VARIANT_TYPE_NAME_LAST         PURC_VARIANT_TYPE_NAME_TUPLE
} purc_variant_type;

#define PURC_VARIANT_TYPE_NR \
    (PURC_VARIANT_TYPE_LAST - PURC_VARIANT_TYPE_FIRST + 1)

...

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
    VARIANT_TYPE_NAME_TUPLE,
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

    /* make sure that the first one is matched */
    assert (strcmp (type_names[PURC_VARIANT_TYPE_FIRST],
                PURC_VARIANT_TYPE_NAME_FIRST) == 0);
    /* make sure that the last one is matched */
    assert (strcmp (type_names[PURC_VARIANT_TYPE_LAST],
                PURC_VARIANT_TYPE_NAME_LAST) == 0);

    return purc_variant_make_string_static (
            type_names [purc_variant_get_type (argv[0])], false);
}
```

	
### 结论

好代码是多次重构出来的！

		
## 更复杂的情形：利用回调函数解耦数据和代码

1) 有如下格式的消息数据包：

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

	
2) 希望解析为如下的数据结构：

```c
typedef enum {
    PCRDR_MSG_TYPE_REQUEST = 0,
    PCRDR_MSG_TYPE_RESPONSE,
    PCRDR_MSG_TYPE_EVENT,
} pcrdr_msg_type;

typedef enum {
    PCRDR_MSG_TARGET_SESSION = 0,
    PCRDR_MSG_TARGET_WINDOW,
    PCRDR_MSG_TARGET_TAB,
    PCRDR_MSG_TARGET_DOM,
} pcrdr_msg_target;

typedef enum {
    PCRDR_MSG_ELEMENT_TYPE_VOID = 0,
    PCRDR_MSG_ELEMENT_TYPE_CSS,
    PCRDR_MSG_ELEMENT_TYPE_XPATH,
    PCRDR_MSG_ELEMENT_TYPE_HANDLE,
} pcrdr_msg_element_type;

pcrdr_msg_type pcrdr_message_get_type(const pcrdr_msg *msg);

typedef enum {
    PCRDR_MSG_DATA_TYPE_VOID = 0,
    PCRDR_MSG_DATA_TYPE_EJSON,
    PCRDR_MSG_DATA_TYPE_TEXT,
} pcrdr_msg_data_type;

struct _pcrdr_msg {
    pcrdr_msg_type          type;
    pcrdr_msg_target        target;
    pcrdr_msg_element_type  elementType;
    pcrdr_msg_data_type     dataType;
    unsigned int            retCode;

    uintptr_t       targetValue;
    char *          operation;
    char *          element;
    char *          property;
    char *          event;

    char *          requestId;

    uintptr_t       resultValue;

    size_t          dataLen;
    char *          data;
};
```

	
### 最初的实现

```c
#define STR_PAIR_SEPARATOR      ":"
#define STR_LINE_SEPARATOR      "\n"
#define STR_VALUE_SEPARATOR     "/"

int pcrdr_parse_packet(char *packet, size_t sz_packet, pcrdr_msg **msg_out)
{
    pcrdr_msg msg;

    char *str1;
    char *line;
    char *saveptr1;

    memset(&msg, 0, sizeof(msg));

    for (str1 = packet; ; str1 = NULL) {
        line = strtok_r(str1, STR_LINE_SEPARATOR, &saveptr1);
        if (line == NULL) {
            goto failed;
        }

        if (is_blank_line(line)) {
            msg.data = strtok_r(NULL, STR_LINE_SEPARATOR, &saveptr1);
            break;
        }

        char *key, *value;
        char *saveptr2;
        key = strtok_r(line, STR_PAIR_SEPARATOR, &saveptr2);
        if (key == NULL) {
            goto failed;
        }

        value = strtok_r(NULL, STR_PAIR_SEPARATOR, &saveptr2);
        if (value == NULL) {
            goto failed;
        }

        if (strcasecmp(key, "type") == 0) {
            ...
        }
        else if (strcasecmp(key, "target") == 0) {
            ...
        }
```

	
### 如何改进？

1. 这不是类似前两个例子那样根据一个键名返回特定数据的情形。
1. 但我们能否将不同键名所做的后续解析处理抽象成一个回调函数？

	
### 改进版本

```c
#define STR_PAIR_SEPARATOR      ":"
#define STR_LINE_SEPARATOR      "\n"
#define STR_VALUE_SEPARATOR     "/"

static bool on_type(pcrdr_msg *msg, char *value)
{
    if (strcasecmp(value, "request")) {
        msg->type = PCRDR_MSG_TYPE_REQUEST;
    }
    else if (strcasecmp(value, "response")) {
        msg->type = PCRDR_MSG_TYPE_RESPONSE;
    }
    else if (strcasecmp(value, "event")) {
        msg->type = PCRDR_MSG_TYPE_EVENT;
    }
    else {
        return false;
    }

    return true;
}

typedef bool (*key_op)(pcrdr_msg *msg, char *value);

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

static struct key_op_pair {
    const char *key;
    key_op      op;
} key_ops[] = {
    { STR_KEY_TYPE,         on_type },
    { STR_KEY_TARGET,       on_target },
    { STR_KEY_OPERATION,    on_operation },
    { STR_KEY_ELEMENT,      on_element },
    { STR_KEY_PROPERTY,     on_property },
    { STR_KEY_EVENT,        on_event },
    { STR_KEY_REQUEST_ID,   on_request_id },
    { STR_KEY_RESULT,       on_result },
    { STR_KEY_DATA_TYPE,    on_data_type },
    { STR_KEY_DATA_LEN,     on_data_len },
};

static key_op find_key_op(const char* key)
{
    for (int i = 0; i < sizeof(key_ops)/sizeof(key_ops[0]); i++) {
        if (strcasecmp(key, key_ops[i].key) == 0) {
            return key_ops[i].op;
        }
    }

    return NULL;
}

int pcrdr_parse_packet(char *packet, size_t sz_packet, pcrdr_msg **msg_out)
{
    pcrdr_msg msg;

    char *str1;
    char *line;
    char *saveptr1;

    memset(&msg, 0, sizeof(msg));

    for (str1 = packet; ; str1 = NULL) {
        line = strtok_r(str1, STR_LINE_SEPARATOR, &saveptr1);
        if (line == NULL) {
            goto failed;
        }

        if (is_blank_line(line)) {
            msg.data = strtok_r(NULL, STR_LINE_SEPARATOR, &saveptr1);
            break;
        }

        char *key, *value;
        char *saveptr2;
        key = strtok_r(line, STR_PAIR_SEPARATOR, &saveptr2);
        if (key == NULL) {
            goto failed;
        }

        value = strtok_r(NULL, STR_PAIR_SEPARATOR, &saveptr2);
        if (value == NULL) {
            goto failed;
        }

        key_op op = find_key_op(key);
        if (op == NULL) {
            goto failed;
        }

        if (!op(&msg, skip_left_spaces(value))) {
            goto failed;
        }
    }

    ...
}
```

	
### 优化

1. 使用二分查找法优化 `find_key_op` 函数
1. 此方法也适用于稀疏整数集合
1. 通用 `sorted-array`：
   - [接口](https://gitee.com/HVML/PurC/blob/master/Source/PurC/include/private/sorted-array.h)
   - [实现](https://gitee.com/HVML/PurC/blob/master/Source/PurC/utils/sorted-array.c)

	
```c
static struct key_op_pair {
    const char *key;
    key_op      op;
} key_ops[] = {
    { STR_KEY_DATA_LEN,     on_data_len },
    { STR_KEY_DATA_TYPE,    on_data_type },
    { STR_KEY_ELEMENT,      on_element },
    { STR_KEY_EVENT,        on_event },
    { STR_KEY_OPERATION,    on_operation },
    { STR_KEY_PROPERTY,     on_property },
    { STR_KEY_REQUEST_ID,   on_request_id },
    { STR_KEY_RESULT,       on_result },
    { STR_KEY_TYPE,         on_type },
    { STR_KEY_TARGET,       on_target },
};

static key_op find_key_op(const char* key)
{
    static ssize_t max = sizeof(key_ops)/sizeof(key_ops[0]) - 1;

    ssize_t low = 0, high = max, mid;
    while (low <= high) {
        int cmp;

        mid = (low + high) / 2;
        cmp = strcasecmp(key, key_ops[mid].key);
        if (cmp == 0) {
            goto found;
        }
        else {
            if (cmp < 0) {
                high = mid - 1;
            }
            else {
                low = mid + 1;
            }
        }
    }

    return NULL;

found:
    return key_ops[mid].op;
}
```

		
## 终极大法

- 用宏生成代码。
- 用脚本生成代码：
   1. 让程序处理大量的重复性编码工作，避免手工编写引入错误
   1. 处理字符串时，字符串较多时，二分法查找性能不高，可以用哈希表；用脚本，可以找到一个相对均衡的哈希算法
   1. 依赖构建系统自动维护代码的重新生成

	
### 用宏生成代码

```c
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
```

	
### 用工具生成代码

- 如何快速获取 CSS 属性（如 `width`、`height`、`color`）的内部属性信息？
- 我们准备一个描述文件
- 然后用 Python 脚本处理这个文件并自动生成代码

	
### 描述文件的内容

- [源文件](https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/include/css/propertylist.txt)

```
background-attachment
    values: scroll fixed inherit
    initial: scroll
    inherited: no

background-color
    values: <color> inherit
    initial: transparent
    inherited: no
...

```

	
### Python 脚本

- [源文件](https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/include/css/make_css_properties.py)

	
### 生成的源文件

- [样式的属性值定义](https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/include/css/csspropertyvalue.inc)
- [样式属性操作函数](https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/include/css/cssdeclared.inc)
- [样式的初始化](https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/src/css/cssinitial.cc)

		
## 作业

- 实现一个代码生成工具，该工具可根据一个文本文件定义的内容，生成用于描述 C 语言关键词、运算符以及语法分隔符的 C 数据结构。要求如下：
   1. 为 C 语言关键词、运算符以及语法分隔符定义足够丰富的结构体信息，可在将来用于生成表达式求值树及抽象语法树。
   1. 可使用 Python 开发或者 C 开发。
   1. 整合到 CMake 构建体系中。

