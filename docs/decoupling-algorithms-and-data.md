## 第六讲：解耦代码和数据

- 时间：2021 年 12 月 16 日 18:30
- 地点：微信视频号“考鼎录”直播间
- [立即预约](#/3)

		
## 解耦代码和数据的重要性

1. 简化代码，降低出错几率
1. 对杂乱的代码做结构化处理
1. 提高代码的可读性和可维护性

	
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

	
## 初学者的实现

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

	
## C标准库的实现

```c
#include <errno.h>

const char * const sys_errlist[] = {
    ...,
    "Invalid argument",     // EINVAL
    ...,
    "Permission denied",    // EACESS
    ...,
};

int sys_nerr;

const char *my_error_message(int errcode)
{
    if (errno < sys_nerr) {
        return sys_errlist[errno];
    }

    return "Bad error code";
}
```


		
## Q & A

