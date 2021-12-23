## 第七讲：子驱动程序

- 时间：2021 年 12 月 23 日 18:30
- 地点：微信视频号“考鼎录”直播间
- [立即预约](#/3)

		
## 开场白：编码水平弱的根本原因

抽象能力不足

1. 对事物的正确认知建立在归纳总结之上
1. 抽象是归纳总结的一种升华
1. 如何提高自己的抽象能力：学好数学
   1. 微积分
   1. 群论
   1. 集合的可数性

		
## 子驱动程序模式在稍有规模的 C 项目中大量应用

1. Unix 的万物皆文件
1. Unix/Linux 内核的虚拟文件系统以及设备驱动程序
1. MiniGUI 中支持多种类型的图片格式以及字体格式

		
## 子驱动程序模式的一般实现套路

1. 一套聚类接口
1. 一些公共数据组成的抽象对象（数据结构）
1. 一组函数指针组成的操作集（数据结构）
1. 针对不同子类的操作集实现

		
## STDIO 接口的实现

```c
struct _file_obj;
typedef struct _file_obj file_obj;

struct _file_ops {
    file_obj *open(void *pathname_buf, size_t size, const char *mode);
    ssize_t read(file_obj *file, void *buf, size_t count);
    ssize_t write(file_obj *file, const void *buf, size_t count);
    off_t lseek(file_obj *file, off_t offset, int whence);
    void close(file_obj *file);
};

struct _FILE;
typedef struct _FILE FILE;
```

## `FILE` 结构：基本设计

```c
struct _FILE {
    struct _file_ops *  ops;
    file_obj *          obj;
};
```

## `fopen` 的实现

```c
struct _file_obj {
    int fd;
};

static file_obj *file_open(void *pathname, size_t size, const char *mode)
{
    (void)size;
    ...
}

static struct _file_ops file_file_ops = {
    open: file_open;
    ...
};

FILE *fopen(const char *pathname, const char *mode)
{
    FILE *file = NULL;

    file_obj *obj = file_open(pathname, 0, mode);
    if (obj) {
        file = calloc(1, sizeof(FILE));
        file.obj = obj;
        file.ops = file_file_ops;
    }

    return file;
}
```

## `fmemopen` 的实现

```c
#define MEM_FILE_FLAG_READABLE      0x01
#define MEM_FILE_FLAG_WRITABLE      0x02

struct _file_obj {
    void *          buf;
    size_t          size;

    unsigned int    flags;
    off_t           rw_pos;
};

static file_obj *mem_open(void *buf, size_t size, const char *mode)
{
    ...
}

static struct _file_ops mem_file_ops = {
    open: mem_open;
    ...
};

FILE *fmemopen(void *buf, size_t size, const char *mode)
{
    FILE *file = NULL;

    file_obj *obj = mem_open(buf, size, mode);
    if (obj) {
        file = calloc(1, sizeof(FILE));
        file.obj = obj;
        file.ops = file_file_ops;
    }

    return file;
}
```

## 进一步考虑

1. STDIO 是带有缓冲区功能的，缓冲区信息应该在 `FILE` 中维护还是在 `file_obj` 中维护？
1. 当前读写位置在什么地方维护？
1. 子驱动程序设计的关键点
   1. 抽象对象的数据结构如何确定？
   1. 操作集如何取舍？

## 正确区分机制和策略

1. 机制：需要提供什么功能
1. 策略：如何使用这些功能
1. 以 STDIO 实现为例：
  - 带有缓冲区支持的格式化输入输出属于使用策略，对不同类型的文件对象是一样的。
  - 文件操作集提供的就是机制，符合最小的完备集合原则。

		
## 下一讲预告

- 主题：动态装载模块
- 地点：微信视频号“考鼎录”直播间
- 时间：2021 年 12 月 30 日 18:30

		
## Q & A

