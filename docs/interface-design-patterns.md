/变体## 第五讲：接口设计模式

- 时间/上：2021 年 11 月 18 日（周四）20:00
- 时间/下：2021 年 11 月 25 日（周四）20:00
- 地点：微信视频号“考鼎录”直播间

		
## 提纲（上）

1. 两个接口设计原则
1. 一般性技巧和方法
1. 模式一：抽象数据类型
1. 模式二：抽象算法
1. 模式三：事件驱动
1. Q & A

		
## 提纲（下）
1. 模式四：隐式上下文
1. 模式五：通用数据结构
1. 模式六：抽象聚类
1. 模式七：接口兼容性
1. Q & A

		
## 两个接口设计原则

1. 完备（completeness）：完整性。
1. 自洽（self-consistency）：符合逻辑，可自圆其说。

	
### 一个例子

为一个独立的网络请求模块设计的接口：

1. 真正从服务器端获取数据的功能运行在独立的网络进程中。
1. 调用者进程启动网络进程，建立套接字连接，发送请求给网络进程。
1. 网络进程处理各种协议的细节，并将结果数据通过另外一个套接字发送给调用者进程。

	
### 初始接口设计

```c
// 为当前 PurC 实例初始化数据获取器模块并创建网络进程。
int pcfetcher_init(size_t max_conns, size_t cache_quota);

// 终止当前 PurC 实例的数据获取器，包括通知网络进程退出，清理缓存的文件等。
int pcfetcher_term(void);

// 创建/销毁会话
struct pcfetcher_session* pcfetcher_session_create(const char* host);
void pcfetcher_session_destroy(struct pcfetcher_session* session);

// 获取网络进程连接标识符。
pcfetcher_conn_id pcfetcher_get_conn_id(void);

// 获取指定会话的连接标识符。
pcfetcher_conn_id pcfetcher_session_get_conn_id(struct pcfetcher_session* session);

// 在指定的会话上设置 COOKIE
void pcfetcher_set_cookie(pcfetcher_session* session, char* path, char* cookies);

// 在指定的会话上获取 COOKIE
char* pcfetcher_get_cookie(pcfetcher_session* session, char* path);

// 响应回调函数的原型
// request_id 是请求标志符。
// ctxt 是发起请求时传入的上下文指针。
// ret_code 是错误码，成功时为 0。
// resp_stream 是响应数据流对象。
typedef void (*response_handler)(purc_variant_t request_id, void* ctxt,
        const struct pcfetcher_resp_header *resp_header,
        purc_rwstream_t resp);

// 发起异步请求，返回一个唯一性的请求标志符，并在有结果时，调用给定的回调函数。
purc_variant_t pcfetcher_request_async(
        struct pcfetcher_session* session,
        const char* url,
        enum pcfetcher_method method,
        purc_variant_t params,
        usigned int timeout,
        response_handler handler, void* ctxt);

// 发起一个同步请求，若成功，返回包含有响应数据的读写流对象。
// resp_header 返回响应头数据。
purc_rwstream_t pcfetcher_request_sync(
        struct pcfetcher_session* session,
        const char* url,
        enum pcfetcher_method method,
        purc_variant_t params, unsigned int timeout,
        struct pcfetcher_resp_header *resp_header);

// 检查是否有来自网络进程的响应数据，如果有响应数据，会调用响应处理回调函数。
// 一般在检测到管道文件描述符或者句柄上有数据可读取时再调用该函数。
int pcfetch_check_response(void);

// 检查是否有来自特定会话上的响应数据，如果有响应数据，会调用响应处理回调函数。
int pcfetcher_check_session_response(struct pcfetcher_session* session);
```

	
### 调整思路

1. 不再暴露会话信息以及数据连接给调用者。在实现上，每个主机对应一个数据连接，使用连接池维护。
1. 调用者无需解析 URL 获取主机信息。

	
### 调整后的设计

```c
// 为当前 PurC 实例初始化数据获取器模块并创建网络进程。
int pcfetcher_init(size_t max_conns, size_t cache_quota);

// 终止当前 PurC 实例的数据获取器，包括通知网络进程退出，清理缓存的文件等。
int pcfetcher_term(void);

// 重置当前 PurC 实例的数据获取器：终止所有未决请求，重置 COOKIE 缓存等。
int pcfetcher_reset(void);

// 设置当前会话的基础 URL，并返回正规域名（FQDN）。
const char* pcfetcher_set_base_url(const char* base_url);

// 设置 COOKIE
void pcfetcher_cookie_set(const char* domain,
        const char* path, const char* name, const char* content,
        time_t expire_time, bool secure);

// 获取 COOKIE
const char* pcfetcher_cookie_get(const char* domain,
        const char* path, const char* name, time_t *expire, bool *secure);

// 移除指定 COOKIE
const char* pcfetcher_cookie_remove(const char* domain,
        const char* path, const char* name);

// 响应回调函数的原型
typedef void (*response_handler)(purc_variant_t request_id, void* ctxt,
        const struct pcfetcher_resp_header *resp_header,
        purc_rwstream_t resp);

// 发起异步请求，返回一个唯一性的请求标志符，并在有结果时，调用给定的回调函数。
purc_variant_t pcfetcher_request_async(
        const char* url,
        enum pcfetcher_method method,
        purc_variant_t params,
        usigned int timeout,
        response_handler handler, void* ctxt);

// 发起一个同步请求，若成功，返回包含有响应数据的读写流对象。
// resp_header 返回响应头数据。
purc_rwstream_t pcfetcher_request_sync(
        struct pcfetcher_session* session,
        const char* url,
        enum pcfetcher_method method,
        purc_variant_t params, unsigned int timeout,
        struct pcfetcher_resp_header *resp_header);

// 检查是否有来自网络进程的响应数据，如果有响应数据，会调用响应处理回调函数。
int pcfetch_check_response(unsigned int timeout_ms);
```

	
### 完备性的保证：对称设计

1. 有 init，必然有 deinit，terminate 或 cleanup
1. 有 create，必然有 destroy
1. 有 open，必然有 close
1. 有 connect，必然有 disconnect
1. 有 read，必然有 write
1. 有 alloc，必然有 free
1. 有 ref，必然有 unref
1. 有 get，必然有 set

	
### 完备性的保证：跟随数据结构

1. 链表，必然有 append, prepend, insertBefore, insertAfter, remove
1. 树，必然有 next、prev、parent、firstChild，lastChild
1. 有序数组，必然有 add，remove, find, get_count, get_member_by_index
1. 哈希表，必然有 add，remove, find, iterator
1. AVL/红黑树，必然有 add, remove, find, iterator

		
## 一般性方法和技巧

### 参数及返回值

1. 明确定义返回值的含义
1. 正确使用 `const` 修饰词
1. 使用最合适的参数类型

```c
void *memcpy(void *dst, const void *src, size_t n);

// 错误的设计
void memcpy(void *dst, void *src, int n);
```

	
### 正确使用常量

[常量的定义和使用](/best-practices-of-c/constants.html)

	
### 使用函数属性

[清除所有编译警告](/best-practices-of-c/warnings.html)

		
## 模式一：抽象数据类型

1. 隐藏实现细节，为增强、优化和可扩展性打下基础
1. 围绕抽象数据结构设计接口
1. 保障接口完备性的方法：对称

	
### 范例一：STDIO 接口

```c
FILE *fopen(const char * path, const char * mode);
FILE *fdopen(int fildes, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);
FILE *fmemopen(void **buf, size_t size, const char *mode);

int fclose(FILE *stream);

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);

int fscanf(FILE *restrict stream, const char *restrict format, ...);
int scanf(const char *restrict format, ...);
int sscanf(const char *restrict s, const char *restrict format, ...);

size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
size_t fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
```

	
### 范例二：抽象读取流接口

HVML 解释器 PurC 版本

```c
struct purc_rwstream;
typedef struct purc_rwstream purc_rwstream;
typedef struct purc_rwstream* purc_rwstream_t;

purc_rwstream_t purc_rwstream_new_from_mem (void* mem, size_t sz);
purc_rwstream_t purc_rwstream_new_from_file (const char* file, const char* mode);
purc_rwstream_t purc_rwstream_new_from_fp (FILE* fp, bool autoclose);

// 使用自动维护的缓冲区创建读写流。主要用于写入内容。
purc_rwstream_t purc_rwstream_new_buffer (size_t sz_init, size_t sz_max);

// Unix-only
purc_rwstream_t purc_rwstream_new_from_unix_fd (int fd, size_t sz_buf);

// Win32-only
purc_rwstream_t purc_rwstream_new_from_win32_socket (int socket, size_t sz_buf);

int purc_rwstream_destroy (purc_rwstream_t rws);

// 定位读写位置
off_t purc_rwstream_seek (purc_rwstream_t rws, off_t offset, int whence);

// 返回当前读写位置；不支持时返回 -1。
off_t purc_rwstream_tell (purc_rwstream_t rws);

// 读取指定数量的字节到缓冲区中。
ssize_t purc_rwstream_read (purc_rwstream_t rws, void* buf, size_t count);

// 按照 UTF-8 编码读取一个合法字符，并转换为 wchar_t，返回其字节长度。
int purc_rwstream_read_utf8_char (purc_rwstream_t rws, char* buf_utf8, wchar_t* buf_wc);

// 写入指定数量的字节。
ssize_t purc_rwstream_write (purc_rwstream_t rws, const void* buf, size_t count);

// 刷新缓冲区。
ssize_t purc_rwstream_flush (purc_rwstream_t rws);

// 从 @in 中读取指定的字节长度，输出到 @out 流中；返回实际转储的字节数量；-1 表示错误。
// @count 为 -1 时，表示转储直到 EOF 为止。
ssize_t purc_rwstream_dump_to_another (purc_rwstream_t in, purc_rwstream_t out, ssize_t count);

// 内存类读写流的缓冲区地址和尺寸；仅针对内存类读写流；其他类型的流返回 NULL 值。
const char* purc_rwstream_get_mem_buffer (purc_rwstream_t rw_mem, size_t *sz);
```

	
其他实现：

- [MiniGUI RWops 接口](https://minigui.fmsoft.cn/api_ref/doc-api-ref-minigui-procs-5.0.6/html/group__general__rw__fns.html)
- [GLib IO Channels 接口](https://developer-old.gnome.org/glib/2.68/glib-IO-Channels.html)

	
### 范例三：一般数据/变体/JSON

HVML 解释器 PurC 的变体接口

```c
struct purc_variant;
typedef struct purc_variant purc_variant;
typedef struct purc_variant* purc_variant_t;

// 在构建变体值时，用于表示无效变体值，意味着出现错误
#define PURC_VARIANT_INVALID            ((purc_variant_t)(0))

// 构造未定义数据
purc_variant_t purc_variant_make_undefined (void);

// 构造空数据
purc_variant_t purc_variant_make_null (void);

// 构造布尔数据
purc_variant_t purc_variant_make_boolean (bool b);

// 构造数值数据
purc_variant_t purc_variant_make_number (double d);

// 构造长整数数据
purc_variant_t purc_variant_make_longint (int64_t i64);

// 构造无符号长整数数据
purc_variant_t purc_variant_make_ulongint (uint64_t u64);

// 构造长双精度浮点数数据
purc_variant_t purc_variant_make_longdouble (long double lf);

// 构造字符串数据，字符串必须是 UTF-8 编码
// 20210707：使用 @check_encoding 参数指定是否检查编码
purc_variant_t purc_variant_make_string (const char* str_utf8, bool check_encoding);

// 获取字符串地址（临时只读）
const char* purc_variant_get_string_const (purc_variant_t value);

// 构造字节序列
purc_variant_t purc_variant_make_byte_sequence (const unsigned char* bytes, size_t nr_bytes);

// 获取字节序列地址及长度（临时只读）
const unsigned char* purc_variant_get_bytes_const (purc_variant_t value, size_t* nr_bytes);

// 构造数组
purc_variant_t purc_variant_make_array (size_t sz, purc_variant_t value0, ...);
bool purc_variant_array_append (purc_variant_t array, purc_variant_t value);
bool purc_variant_array_prepend (purc_variant_t array, purc_variant_t value);
bool purc_variant_array_set (purc_variant_t array, int idx, purc_variant_t value);
bool purc_variant_array_remove (purc_variant_t array, int idx);
bool purc_variant_array_insert_before (purc_variant_t array, int idx, purc_variant_t value);
bool purc_variant_array_insert_after (purc_variant_t array, int idx, purc_variant_t value);

// 获取数组元素数量
size_t purc_variant_array_get_size (purc_variant_t array);
// 获取指定数组元素
purc_variant_t purc_variant_array_get (purc_variant_t array, int idx);

...

// 引用变体值。引用计数加 1；返回操作后的引用计数
unsigned int purc_variant_ref (purc_variant_t value);

// 反引用变体值。引用计数减 1；返回操作后的引用计数；当引用计数为 0 时，释放资源
unsigned int purc_variant_unref (purc_variant_t value);
```

	
其他实现：

- [json-c](http://json-c.github.io/json-c/json-c-0.15/doc/html/json__object_8h.html)
- [JerryScript](https://jerryscript.net/api-reference/)

		
## 模式二：抽象算法

1. 围绕抽象算法设计接口
1. 算法不依赖于具体的数据类型
1. 算法不依赖于具体的数据存储方式

	
### 范例一：STDC 的 `qsort` 函数

```c
void qsort(void *base, size_t nel, size_t width,
        int (*compar)(const void *, const void *));
```

	
### 不依赖于连续存储的版本

```c
void qsort_ex(void *array,
        void * (*get_member)(void *, int idx),
        void (*exchange_members)(void *, int idx_one, int idx_oth),
        int (*compar)(const void *, const void *));
```

	
### 范例二：MiniGUI 的曲线生成器函数

```c
typedef void (* CB_LINE) (void* context,
        int stepx, int stepy);

void GUIAPI LineGenerator (void* context,
        int x1, int y1, int x2, int y2, CB_LINE cb);

typedef void (* CB_CIRCLE) (void* context,
        int x1, int x2, int y);
void GUIAPI CircleGenerator (void* context,
        int sx, int sy, int r, CB_CIRCLE cb);
```

	
### 用圆生成器绘制圆

```c

void _dc_set_pixel_pair_clip (void* context,
        int x1, int x2, int y);

void GUIAPI Circle (HDC hdc, int sx, int sy, int r)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;

    if (r < 1) {
        _gdi_set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - r;
    pdc->rc_output.top  = sy - r;
    pdc->rc_output.right = sx + r + 1;
    pdc->rc_output.bottom = sy + r + 1;

    ENTER_DRAWING (pdc);

    CircleGenerator (pdc, sx, sy, r, _dc_set_pixel_pair_clip);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}
```

	
### 用圆生成器生成圆剪切域

```c
static void cb_region (void* context, int x1, int x2, int y)
{
    CLIPRGN* region = (CLIPRGN*) context;
    CLIPRGN newregion;
    CLIPRECT newcliprect;

    if (x2 > x1) {
        newcliprect.rc.left = x1;
        newcliprect.rc.right = x2 + 1;
    }
    else {
        newcliprect.rc.left = x2;
        newcliprect.rc.right = x1 + 1;
    }
    newcliprect.rc.top = y;
    newcliprect.rc.bottom = y + 1;

    newcliprect.next = NULL;
    newcliprect.prev = NULL;

    newregion.type = SIMPLEREGION;
    newregion.rcBound = newcliprect.rc;
    newregion.head = &newcliprect;
    newregion.tail = &newcliprect;
    newregion.heap = NULL;

    if (IsEmptyClipRgn (region))
        CopyRegion (region, &newregion);
    else
        UnionRegion (region, region, &newregion);
}

BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r)
{
    EmptyClipRgn (dst);

    if (r < 1) {
        CLIPRECT* newcliprect;

        NEWCLIPRECT (dst, newcliprect);
        newcliprect->rc.left = x;
        newcliprect->rc.top = y;
        newcliprect->rc.right = x + 1;
        newcliprect->rc.bottom = y + 1;
        return TRUE;
    }

    CircleGenerator (dst, x, y, r, cb_region);

    return TRUE;
}
```

		
## 模式三：事件驱动

1. 事件循环及消息队列
1. 回调函数的粒度（granularity）

		
## Q & A

		
## 模式四：隐式上下文

1. 上下文本质上是个全局变量
1. 使用线程局部存储（TLS）实现线程安全

		
## 模式五：通用数据结构

1. 在算法数据结构中保留用户数据字段
1. 在用户数据结构中包含算法数据结构
1. 迭代函数或迭代宏

		
## 模式六：抽象聚类

1. 抽象操作集
1. 注册及吊销子类

		
## 模式七：接口兼容性

1. 新增接口而非修改原有接口的语义
1. 二进制兼容或源代码兼容
1. 宏或者内联函数实现接口的向后兼容性

		
## Q & A
