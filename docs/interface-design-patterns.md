## 第五讲：接口设计模式

1. 何谓好接口？
1. 两个接口设计原则
1. 一般性技巧和方法
1. 模式一：抽象数据类型
1. 模式二：抽象算法
1. 模式三：上下文
1. 模式四：事件驱动
1. 模式五：通用数据结构
1. 模式六：同类聚合
1. 模式七：遍历和迭代
1. 模式八：接口的扩展和兼容性

		
## 何谓好接口？

1. 恰当的抽象，比如
   - POSIX 的 文件描述符
   - POSIX 的 DIRENT 结构
   - STDC 的 FILE 结构
1. 调用者友好
1. 符合惯例，学习成本低
1. 没有过度设计
1. 稳定的接口设计有助提升
   - 软件质量
   - 可维护性

	
### 例子：从 `select` 到 `epoll`

1. `select` 的问题：
   - 需要静态的 `fd_set` 来维护被监听的文件描述符，每次调用前要重置 `fd_set`。
   - 调用者需要自行维护文件描述符和更高层软件对象之间的对应关系。
1. `epoll` 的改进：
   - 将监听多个文件描述符的行为抽象为单个文件描述符上的监听。
   - 由内核维护被监听文件描述符和更高层软件对象（调用者指定的指针）之间的对应关系。

		
## 两个接口设计原则

1. 完备（completeness）：完整，不缺东西。
1. 自洽（self-consistency）：无逻辑漏洞，自圆其说。

	
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
int pcfetcher_check_response(void);

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
        const char* url,
        enum pcfetcher_method method,
        purc_variant_t params, unsigned int timeout,
        struct pcfetcher_resp_header *resp_header);

// 检查是否有来自网络进程的响应数据，如果有响应数据，会调用响应处理回调函数。
int pcfetcher_check_response(unsigned int timeout_ms);
```

	
### 完备性的保证：对称设计

1. 有 new，必然有 delete
1. 有 init，必然有 deinit、terminate 或 cleanup
1. 有 create，必然有 destroy
1. 有 open，必然有 close
1. 有 connect，必然有 disconnect
1. 有 read，必然有 write
1. 有 alloc，必然有 free
1. 有 ref，必然有 unref
1. 有 get，必然有 set
1. ……

	
### 完备性的保证：跟随数据结构

1. 链表，必然有 append、prepend、insertBefore、insertAfter、remove
1. 树，必然有 next、prev、parent、firstChild、lastChild
1. 有序数组，必然有 add，remove、find、getCount、getMemberByIndex
1. 哈希表，必然有 add，remove、find、iterator
1. AVL/红黑树，必然有 add、remove、find、iterator
1. ……

		
## 一般性方法和技巧

### 参数及返回值

1. 尽量避免返回 `void`
1. 明确定义返回值的含义
1. 正确使用 `const` 修饰词
1. 使用最合适的参数类型

```c
void *memcpy(void *dst, const void *src, size_t n);

// 错误的设计
void my_memcpy(void *dst, void *src, int n);
```

	
### 正确使用常量

[常量的定义和使用](/best-practices-of-c/constants.html)

	
### 使用函数属性

[清除所有编译警告](/best-practices-of-c/warnings.html)

		
## 模式一：抽象数据类型

1. 隐藏实现细节，为增强、优化和可扩展性打下基础
1. 围绕抽象数据结构设计接口

	
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
void qsort_ex(void *array, size_t nel,
        void * (*get_member)(void *, size_t idx),
        void (*exchange_members)(void *, size_t idx_one, size_t idx_oth),
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

		
## 实例研究：sorted array

- 可按照一个给定的排序值（sort value）排序。
- 支持添加（add）和移除（remove）操作。
- 可查找（find）给定的排序值是否存在。
- 可根据索引值线性访问。

	
### PurC 中的 sorted array 接口

```c
struct sorted_array;

typedef void (*sacb_free)(void *sortv, void *data);
typedef int  (*sacb_compare)(const void *sortv1, const void *sortv2);

#define SAFLAG_ORDER_ASC            0x0000
#define SAFLAG_ORDER_DESC           0x0001
#define SAFLAG_DUPLCATE_SORTV       0x0002

#define SAFLAG_DEFAULT              0x0000

#ifdef __cplusplus
extern "C" {
#endif

/* create an empty sorted array; free_fn can be NULL */
struct sorted_array *
sorted_array_create(unsigned int flags, size_t sz_init,
        sacb_free free_fn, sacb_compare cmp_fn);

/* destroy a sorted array */
void sorted_array_destroy(struct sorted_array *sa);

/* add a new member with the sort value and the data. */
int sorted_array_add(struct sorted_array *sa, void *sortv, void *data);

/* remove one member which has the same sort value. */
bool sorted_array_remove(struct sorted_array *sa, const void* sortv);

/* find the first member which has the same sort value. */
bool sorted_array_find(struct sorted_array *sa,
        const void *sortv, void **data);

/* retrieve the number of the members of the sorted array */
size_t sorted_array_count(struct sorted_array *sa);

/* retrieve the member by the index and return the sort value;
   data can be NULL. */
const void* sorted_array_get(struct sorted_array *sa, size_t idx, void **data);

/* delete the member by the index */
void sorted_array_delete(struct sorted_array *sa, size_t idx);

#ifdef __cplusplus
}
#endif
```

		
## 模式三：隐式上下文

- 上下文通常用于保存当前的设置、状态等信息，通常被设计为句柄（handle）、隐藏细节的结构指针等。

- 上下文在图形绘制接口中常见，比如在 MiniGUI 或者 Cairo 中绘制一个矩形。

	
### 显式上下文

MiniGUI:

```c
    hdc = BeginPaint (hWnd);

    // Draw a rectangle.
    MoveTo (hdc, 0, 0);
    LineTo (hdc, 0, 100);
    LineTo (hdc, 100, 100);
    LineTo (hdc, 100, 0);
    LineTo (hdc, 0, 0);

    EndPaint (hWnd, hdc);
```

	
Cairo:

```c
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 120, 120);
    cr = cairo_create (surface);

    cairo_move_to (cr, 0.0, 0.0);
    cairo_move_to (cr, 0.0, 0.5);
    cairo_line_to (cr, 0.5, 0.5);
    cairo_line_to (cr, 0, 0.5);
    cairo_line_to (cr, 0, 0);
```

	
### MiniGUI 的上下文结构细节

```c
struct tagDC
{
    unsigned char DataType;  /* the data type, always be TYPE_HDC */
    unsigned char DCType;    /* the dc type */

    unsigned char bInUse;
    unsigned char bIsClient;

    HWND hwnd;

    /* surface of this DC */
    GAL_Surface* surface;

    /* background color */
    gal_pixel bkcolor;

    /* pen color */
    gal_pixel pencolor;

    /* solid brush color */
    gal_pixel brushcolor;

    /* text color */
    gal_pixel textcolor;

    int bkmode;

    int tabstop;
    int cExtra;     /* Character extra */
    int wExtra;     /* Word extra */
    int alExtra;    /* Above line extra */
    int blExtra;    /* Bellow line extra */

    int mapmode;    /* mappping mode */

    int ta_flags;   /* Text alignment flags */

    int bidi_flags; /* BIDI flags */

    /* pen attributes */
    int pen_type;
    int pen_cap_style;
    int pen_join_style;
    unsigned int pen_width;

    /* brush attributes */
    int brush_type;

    POINT brush_orig;
    const BITMAP* brush_tile;
    const STIPPLE* brush_stipple;

    /* custom dash info */
    int dash_offset;
    const unsigned char* dash_list;
    size_t dash_list_len;

    PLOGFONT pLogFont;

    POINT CurPenPos;
    POINT CurTextPos;

    POINT ViewOrig;
    POINT ViewExtent;
    POINT WindowOrig;
    POINT WindowExtent;

    /* raster operation */
    int rop;

    /* used by the text rendering for anti-aliasing fonts. */
    gal_pixel gray_pixels [17];
    /* used by the text rendering for low-pass filtering. */
    gal_pixel filter_pixels [17];
    GAL_PixelFormat* alpha_pixel_format;

    /* pixel and line operation */
    CB_COMP_SETPIXEL draw_pixel;
    CB_COMP_SETHLINE draw_pixel_span;
    CB_COMP_PUTHLINE draw_src_span;
    DC_MOVE_TO move_to;
    DC_STEP_X  step_x;

    gal_uint8*  cur_dst;
    void*       user_comp_ctxt;
    gal_pixel   skip_pixel;
    gal_pixel   cur_pixel;
    int         step;

    CLIPRECT* cur_ban;
    RECT rc_output;

    /* local clip region information */
    CLIPRGN  lcrgn;

    /* effective clip region information */
    CLIPRGN  ecrgn;

    /* device rect */
    RECT DevRC;

    PGCRINFO pGCRInfo;
    unsigned int oldage;

    CB_BITMAP_SCALER_FUNC bitmap_scaler;
};
```

	
### 使用隐式上下文的情形

使用 OpenGL API 绘制矩形：

```c
glColor3f(1.0, 0.0, 0.0);

glBegin(GL_LINE_LOOP);
glVertex3f(0.0, 0.0, 0.0);
glVertex3f(0.0, 1.0, 0.0);
glVertex3f(1.0, 1.0, 0.0);
glVertex3f(1.0, 0.0, 0.0);
glEnd();
glFlush();
```

我们发现，上下文句柄或者指针不见了。

	
### 没有上下文信息……

1. 是不是意味着无法同时在一个进程的两个窗口（或表面）中绘制不同的图形？
1. 是不是意味着无法在同一个进程中的不同线程中绘制不同的图形？

	
### OpenGL 中的隐式上下文

1. 使用线程本地存储（TLS）保存上下文信息。应用程序无需关注默认上下文的创建及销毁。
1. 在同一个线程内，使用 `eglCreateContext` 创建多个上下文，使用 `eglMakeCurrent` 函数切换上下文。

	
### 隐式上下文的好处

1. 减少函数中的参数传递，尤其是上下文和线程绑定时。
1. 解决接口的历史遗留问题。

	
### 实例1

HVML 解释器 PurC 中的变体管理接口：

```c
purc_variant_t purc_variant_make_number(double d);
```

其实现：

```c
// 变体堆和 PurC 实例关联
struct pcinst {
    int errcode;

    char* app_name;
    char* runner_name;

    pcutils_map* local_data_map;

    struct pcvariant_heap variant_heap;
};

/* gets the current instance */
struct pcinst* pcinst_current(void) WTF_INTERNAL;

// 使用 struct pcinst 指针作为线程本地存储
struct pcinst* pcinst_current(void)
{
    struct pcinst* curr_inst;
    curr_inst = PURC_GET_THREAD_LOCAL(inst);

    // 未初始化 PurC 实例的情形。
    if (curr_inst == NULL || curr_inst->app_name == NULL) {
        return NULL;
    }

    return curr_inst;
}
```

	
### 实例2

标准 C 库的错误码：`errno`

文档中是 `errno` 是一个全局变量：

```c
extern int errno;
```

但实际却被定义为：

```c
extern int * __error(void);
#define errno (*__error())
```

	
### 相关术语

- explicit（显式的）
- implicit（隐式的）
- context（上下文）
- TLS（Thread Local Storage，线程本地存储）


		
## 模式四：事件/消息驱动

1. 事件/消息驱动最早出现在 GUI 编程中，如 Win32 和各种 GUI 库，用于处理人机交互事件、窗口时间等。
1. 在 Glib、WTF、MiniGUI 当中，被进一步抽象，可用来监听文件描述符（包括套接字和管道）、定时器以及用户定制的事件。
1. 还可用作线程间通讯机制使用。

	
### 事件/消息驱动接口的基本概念

1. 事件/消息生产者
1. 事件/消息消费者
1. 事件/消息处理器（回调函数）

	
### MiniGUI 消息驱动（1/2）

1. 消息驱动接口围绕窗口设计，每个窗口有一个自己的消息处理回调函数。
1. 在 MiniGUI 多线程模式下，每个线程可以创建一个自己的消息队列。
1. 一个消息由一个整数标识符和两个参数组成。

	
### MiniGUI 消息驱动（2/2）
1. 消息的产生者，可通过 `PostMessage`、`SendNotifyMessage` 和 `SendMessage` 三个接口产生消息：
   - 邮寄消息，使用循环队列存储，可能会溢出（丢失）。
   - 通知消息，使用链表存储，不会丢失。
   - 发送消息，同步等待消息的处理并获得处理返回值。
1. 消息的消费者，通过窗口消息处理回调函数接收消息并进行处理。

	
### MiniGUI 消息驱动主要接口

```c
/* 消息的产生接口 */
int GUIAPI PostMessage (HWND hWnd,
        UINT nMsg, WPARAM wParam, LPARAM lParam);
LRESULT GUIAPI SendMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);
int GUIAPI SendNotifyMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);
int GUIAPI PostQuitMessage (HWND hWnd);

/* 消息的获取和处理接口 */
BOOL GUIAPI GetMessage (PMSG pMsg, HWND hWnd);
LRESULT GUIAPI DispatchMessage (PMSG pMsg);
```

	
### MiniGUI 消息处理函数及消息循环

```c
static LRESULT HelloWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            TextOut (hdc, 0, 0, "Hello, world!");
            EndPaint (hWnd, hdc);
        break;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

    ...

    HWND hMainWnd = CreateMainWindow (...);
    while (GetMessage (&Msg, hMainWnd)) {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }
```

	
### MiniGUI 处理定时器和监听文件描述符

```c
/* 被监听的文件描述符产生可读、可写或者异常事件时，
   会产生 MSG_FDEVENT 消息到指定的窗口。 */
BOOL GUIAPI RegisterListenFD (int fd, int type,
                HWND hwnd, void* context);
BOOL GUIAPI UnregisterListenFD (int fd);

/* 设置的定时器到期时，会产生 MSG_TIMER 消息到指定的窗口。*/
BOOL GUIAPI SetTimer (HWND hWnd, LINT id, DWORD speed);
int GUIAPI KillTimer (HWND hWnd, LINT id);
```

	
### MiniGUI 虚拟窗口

虚拟窗口拥有窗口标识符和窗口过程，可以接受一般性消息。用于在非 GUI 线程中处理消息。

```c
HWND GUIAPI CreateVirtualWindow (HWND hHosting, WNDPROC WndProc,
        const char* spCaption, LINT id, DWORD dwAddData);
```

	
### Glib 的事件驱动接口

```c
static GMainLoop* loop;
static gint counter = 10;

static gboolean my_timer_callback(gpointer arg)
{
    if (--counter == 0) {
        g_main_loop_quit (loop);
        return FALSE;
    }

    return TRUE;
}

    loop = g_main_loop_new(NULL, FALSE);

    /* duration: 1000ms */
    g_timeout_add(1000, my_timer_callback, NULL);

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
```

	
### Glib 监听文件描述符

```c
static GMainLoop* loop;
static gboolean my_callback(GIOChannel *channel)
{
    // 处理 GIOChannel 上的可读数据
    ...
}

    // 新建一个 GMainContext 上下文对象
    GMainContext* context = g_main_context_new();

    // 在给定的文件描述符上创建一个 GIOChannel 对象
    GIOChannel* channel = g_io_channel_unix_new(fd);

    // 在上面创建的 GIOChannel 对象基础上创建一个可监听的数据源
    GSource* source = g_io_create_watch(channel, G_IO_IN);
    g_io_channel_unref(channel);

    //设置数据源上可读时，要调用的回调函数
    g_source_set_callback(source, (GSourceFunc)my_callback, channel, NULL);

    // 将 GSource 附加到 GMainContext 对象上
    g_source_attach(source, context);
    g_source_unref(source);

    /* 使用 GMainContext 对象创建 GMainLoop 对象
    loop = g_main_loop_new(context, FALSE);

    // 进入事件循环
    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_main_context_unref(context);
```

	
### 事件/消息处理器的粒度（granularity）

1. 粗粒度：一个事件处理器处理所有的事件
   - 简洁，但需要自行析构参数，消息回调函数的代码冗长。
1. 细粒度：一个事件处理器处理指定的事件
   - 直接，但需要更多内存保存事件和事件处理器之间的映射关系。

	
### 细化粗粒度的事件处理器

建立消息及消息处理器的映射表：

```c
typedef BOOL (* TIMERPROC)(HWND, LINT, DWORD);
BOOL GUIAPI SetTimerEx (HWND hWnd, LINT id, DWORD speed,
        TIMERPROC timer_proc);
```

	
或封装为 C++ 的类，使用面向对象实现：

```c++
class Window : public Object {
public:
    Window();
    virtual ~Window();

    /* public methods */
    // create a main window for HybridOS View document
    bool create(HWND hosting, int x, int y, int w, int h,
        bool visible = true);
    // create a control for HybridOS View document
    bool create(HWND parent, int x, int y, int w, int h,
        bool visible, int id);

    void setWindowRect(const IntRect& rc);
    // get client rect in physical pixels
    void getClientRect(IntRect& rc) const;
    // get client rect in logical pixels
    void getClientRect(RealRect& rc) const;

    void show(bool updateBg = true);
    void hide();
    void destroy();

    HWND getSysWindow() const { return m_sysWnd; }
    static Window* getObject(HWND hwnd);

    HWND setActiveWindow(HWND hwnd);
    HWND getActiveWindow();

    void updateWindow(bool updateBg = true);
    void asyncUpdateRect(int x, int y, int w, int h, bool updateBg = true);
    void syncUpdateRect(int x, int y, int w, int h, bool updateBg = true);

    unsigned int doModalView();

    int doModal(bool bAutoDestory = true);
    void endDlg(int endCode);

    RootView* getRootView() { return m_rootView; }
    bool setRootView(RootView* root);

    /* event handlers */
    virtual bool onKeyEvent(const KeyEvent* event);
    virtual bool onMouseEvent(const MouseEvent* event);
    virtual bool onMouseWheelEvent(const MouseWheelEvent* event);
    virtual bool onIdle() { return false; }

    // you can overload the method to define customized keycode.
    virtual KeyEvent::KeyCode scancode2keycode(int scancode);

    static bool RegisterHVRootControl();
    static bool UnregisterHVRootControl();

    /* to be deprecated */
    virtual void drawBackground(GraphicsContext* gc, IntRect &rc);

protected:
    HWND m_sysWnd;
    RootView* m_rootView;

    LRESULT commWindowProc(HWND hWnd, UINT message,
            WPARAM wParam, LPARAM lParam);
    static LRESULT defaultMainWindowProc(HWND hWnd, UINT message,
            WPARAM wParam, LPARAM lParam);
    static LRESULT defaultControlProc(HWND hWnd, UINT message,
            WPARAM wParam, LPARAM lParam);

    /* helpers to handle event */
    int onKeyMessage(KeyEvent::KeyEventType keytype,
        WPARAM wParam, LPARAM lParam);
    int onMouseMessage(MouseEvent::MouseEventType mouseType,
        WPARAM wParam, LPARAM lParam);
};

LRESULT Window::commWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_KEYDOWN:
        return onKeyMessage(KeyEvent::KEY_DOWN, wParam, lParam);

    case MSG_KEYUP:
        return onKeyMessage(KeyEvent::KEY_UP, wParam, lParam);

    case MSG_KEYLONGPRESS:
        return onKeyMessage(KeyEvent::KEY_LONGPRESSED, wParam, lParam);

    case MSG_KEYALWAYSPRESS:
        return onKeyMessage(KeyEvent::KEY_ALWAYSPRESS, wParam, lParam);

    case MSG_LBUTTONDOWN:
        return onMouseMessage(MouseEvent::MOUSE_L_DOWN, wParam, lParam);

    case MSG_LBUTTONUP:
        return onMouseMessage(MouseEvent::MOUSE_L_UP, wParam, lParam);

    case MSG_MOUSEMOVE:
        return onMouseMessage(MouseEvent::MOUSE_MOVE, wParam, lParam);

    case MSG_IDLE:
        onIdle();
        return 0;

    case MSG_ERASEBKGND:
        return 0;

    case MSG_PAINT: {
        HDC hdc = BeginPaint(hWnd);
        ...
        EndPaint(hWnd, hdc);
        SyncUpdateDC(HDC_SCREEN);
        return 0;
    }

    default:
        break;
    }

    return 1;
}

LRESULT Window::defaultMainWindowProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    Window* wnd = Window::getObject(hWnd);
    if (wnd == NULL) {
        _DBG_PRINTF("Window::defaultMainWindowProc: wnd is NULL\n");
        return DefaultMainWinProc(hWnd, message, wParam, lParam);
    }

    if (wnd->commWindowProc(hWnd, message, wParam, lParam))
        return DefaultMainWinProc(hWnd, message, wParam, lParam);

    return 0;
}
```
	

### 进一步研究

1. Gtk 的 `signal` 机制及相关接口
1. WTF 的 `RunLoop` 接口

	
### 相关术语

- event（事件）
- message（消息）
- event/message-driven（事件/消息驱动）
- callback（回调函数）
- handler（处理器）
- event/message loop（事件/消息循环）
- signal（信号）
- sync.（同步；同步的）
- async.（异步；异步的）

		
## 模式五：通用数据结构


	
### 在算法数据结构中保留用户数据字段

以树形结构为例：

```c
typedef struct purc_tree_node {
    /* user data */
    void* user_data;

    /* number of children */
    size_t nr_children;

    /* the parent */
    struct purc_tree_node *parent;

    /* the first and last children */
    struct purc_tree_node *first, *last;

    /* the next and previous siblings */
    struct purc_tree_node *next, *prev;
} purc_tree_node;

typedef struct purc_tree_node* purc_tree_node_t;

// 设置节点上的用户数据
void purc_tree_node_set_user_data (purc_tree_node_t node, void *user_data);

// 获取节点上的用户数据
void *purc_tree_node_get_user_data (purc_tree_node_t node);

// 将一个节点追加为指定节点的最后一个子节点
bool purc_tree_node_append_child (purc_tree_node_t parent, purc_tree_node_t node);

// 将一个节点追加为指定节点的第一个子节点
bool purc_tree_node_prepend_child (purc_tree_node_t parent, purc_tree_node_t node);

// 将一个节点插入到给定节点的前面
bool purc_tree_node_insert_before (purc_tree_node_t current, purc_tree_node_t node);

// 将一个节点插入到给定节点的后面
bool purc_tree_node_insert_after (purc_tree_node_t current, purc_tree_node_t node);

// 获取给定节点的父节点
purc_tree_node_t purc_tree_node_parent (purc_tree_node_t node);

// 获取给定节点的第一个子节点
purc_tree_node_t purc_tree_node_child (purc_tree_node_t node);

// 获取给定节点的最后一个子节点
purc_tree_node_t purc_tree_node_last_child (purc_tree_node_t node);

// 获取给定节点的下一个兄弟子节点
purc_tree_node_t purc_tree_node_next (purc_tree_node_t node);

// 获取给定节点的上一个兄弟子节点
purc_tree_node_t purc_tree_node_prev (purc_tree_node_t node);
```

	
### 在用户数据结构中包含节点数据结构

- 一次内存分配即可。
- 简洁，性能高，通用性好。
- 广泛应用于使用节点的各类数据结构，如链表、树、二叉树、AVL 树、红黑树等。

	
### 以双向链表为例

节点数据结构：

```c
struct list_head {
    struct list_head *next;
    struct list_head *prev;
};
```

	
使用时：

```c
typedef struct _pattern_list {
    struct list_head list;
    int nr_patterns;
} pattern_list;

struct one_pattern {
    struct list_head  list;

    int type;
    union {
        char*         var_name;
        GPatternSpec* spec;
        GPatternSpec* not_spec;
    };
};
```

	
### 主要接口

```c
static inline void
INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list->prev = list;
}

static inline void
_list_add(struct list_head *_new, struct list_head *prev,
    struct list_head *next)
{

    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static inline void
list_add(struct list_head *_new, struct list_head *head)
{
    _list_add(_new, head, head->next);
}

static inline void
list_add_tail(struct list_head *_new, struct list_head *head)
{
    _list_add(_new, head->prev, head);
}

#define    list_entry(ptr, type, field)    container_of(ptr, type, field)
#define    list_first_entry(ptr, type, field)    list_entry((ptr)->next, type, field)
#define    list_last_entry(ptr, type, field)    list_entry((ptr)->prev, type, field)
```

	
### `container_of` 宏

从成员指针获得成员所在容器的指针：

```c
#define container_of(ptr, type, member)                                 \
    ({                                                                  \
        const __typeof__(((type *) NULL)->member) *__mptr = (ptr);      \
        (type *) (void*) (((char *)__mptr) - offsetof(type, member));   \
    })
#endif
```

	
### 迭代宏

```c
#define    list_for_each(p, head)                               \
    for (p = (head)->next; p != (head); p = p->next)

#define    list_for_each_safe(p, n, head)                       \
    for (p = (head)->next, n = p->next; p != (head); p = n, n = p->next)

#define list_for_each_entry(p, h, field)                                        \
    for (p = list_first_entry(h, __typeof__(*p), field); &p->field != (h);      \
        p = list_entry(p->field.next, __typeof__(*p), field))

#define list_for_each_entry_safe(p, n, h, field)                                \
    for (p = list_first_entry(h, __typeof__(*p), field),                        \
        n = list_entry(p->field.next, __typeof__(*p), field); &p->field != (h); \
        p = n, n = list_entry(n->field.next, __typeof__(*n), field))

#define    list_for_each_entry_reverse(p, h, field)                             \
    for (p = list_last_entry(h, __typeof__(*p), field); &p->field != (h);       \
        p = list_entry(p->field.prev, __typeof__(*p), field))

#define  list_for_each_prev(p, h) for (p = (h)->prev; p != (h); p = p->prev)
#define  list_for_each_prev_safe(p, n, h) for (p = (h)->prev, n = p->prev; p != (h); p = n, n = p->prev)
```

	
### 迭代宏的用法

```c
void cleanup_pattern_list (pattern_list *pl)
{
    struct list_head *node, *tmp;
    struct one_pattern *pattern;

    if (pl->nr_patterns == 0)
        return;

    list_for_each_safe (node, tmp, &pl->list) {
        pattern = (struct one_pattern *)node;

        switch (pattern->type) {
            case PT_ANY:
                break;

            case PT_SPEC:
                assert (pattern->spec);
                g_pattern_spec_free (pattern->spec);
                break;

            case PT_NOT_SPEC:
                assert (pattern->not_spec);
                g_pattern_spec_free (pattern->not_spec);
                break;

            case PT_VARIABLE:
                assert (pattern->var_name);
                free (pattern->var_name);
                break;
        }

        free (pattern);
    }
}
```

	
### 其他通用数据结构

1. AVL 树  
<https://gitlab.fmsoft.cn/hybridos/hibox/blob/master/src/avl.h>
1. 红黑树  
<https://gitlab.fmsoft.cn/hybridos/hybridos/blob/master/device-side/hfcl/include/common/rbtree.h>

		
## 模式六：同类聚合

什么叫同类聚合？

	
### 标准IO接口设计中的同类聚合（1/2）

- 除了我们熟知的普通文件，我们还可以将内存块视作输入输出流：

```c
#include <stdio.h>

FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *stream);

FILE *fmemopen(void *buf, size_t size, const char *mode);
```

	
### 同类聚合带来的好处

1. 读写接口可同时作用于文件和内存块。
1. 提高代码重用率（可维护性）。

	
### 标准IO接口设计中的同类聚合（2/2）

- 使用格式化字符串将各种数据类型的格式化输入和输出统一成了两个基本接口：

```c
#include <stdio.h>

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);

#include <stdarg.h>

int vfprintf(FILE *stream, const char *format, va_list ap);
```

	
### 同类聚合带来的好处

1. 简化接口的设计，降低学习成本。
1. 灵活性：一个接口处理所有数据类型；一个接口处理多个数据。
1. 可扩展性：增加新的格式化记号，不需要增加接口。

	
### MiniGUI 中装载不同图片格式的接口

```c
int LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap,
                const char* spFileName);
int LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap,
                const void* mem, size_t size, const char* ext);

int LoadBitmapEx (HDC hdc, PBITMAP pBitmap,
                MG_RWops* area, const char* ext);

const char* CheckBitmapType (MG_RWops* rwstream);
```

	
### 同类聚合带来的好处

1. 通过后缀名来确定装载的图片格式；后缀名失效时使用 `CheckBitmapType`。
1. 可从文件或内存中装载；底层使用一个类似 STDIO FILE * 的抽象读写流对象。
1. 支持新的图片格式时，无需新增新的接口。

		
## 模式七：遍历和迭代

	
### 方法一：遍历宏

一个简单的分词器接口：

```c
/** 获取字符串中下一个合法词元的起始指针和长度；其中参数 delims 表示可能的词元
    分隔符。该函数返回 NULL 表示没有合法词元；若返回值不为 NULL，则通过
    token_len 指针返回下个合法词元的长度。 */
PCA_EXPORT const char *
pcutils_get_next_token_len(const char *str, size_t str_len,
        const char *delims, size_t *token_len);
```

	
常规用法：

```c
#define _KW_DELIMITERS      " \t\n\v\f\r"       // 常用词元分隔符

static void handle_options(const char *options, size_t options_len)
{
    size_t kwlen = 0;
    const char *keyword = pcutils_get_next_token_len(options, options_len,
            _KW_DELIMITERS, &kwlen);
    while (keyword != NULL) {

        if (kwlen == 4 && strncasecmp(keyword, "read", kwlen) == 0) {
            /* 处理关键词 read */
            ...
        }
        else if (kwlen == 5 && strncasecmp(keyword, "write", kwlen) == 0) {
            /* 处理关键词 write */
            ...
        }
        else {
            /* 其他关键词 */
            ...
        }

        /* 获取下个关键词 */
        options_len -= kwlen;
        keyword = pcutils_get_next_token_len(keyword + kwlen, options_len,
                _KW_DELIMITERS, &kwlen);
    }
}
```

	
遍历宏：

```c
#define PURC_KW_DELIMITERS      " \t\n\v\f\r"
#define MAX_LEN_KEYWORD         64

#define for_each_keyword(options, total_len, kw, kw_len)                    \
    for (kw_len = 0, kw = pcutils_get_next_token_len(options, total_len,    \
                PURC_KW_DELIMITERS, &kw_len);                               \
            (kw != NULL && kw_len > 0 && kw_len < MAX_LEN_KEYWORD);         \
            total_len -= kw_len,                                            \
            kw = pcutils_get_next_token_len(kw + kw_len,                    \
                total_len, PURC_KW_DELIMITERS, &kw_len))
...

    const char *keyword;
    size_t kwlen;
    for_each_keyword(options, total_len, keyword, kwlen) {
        if (kwlen == 4 && strncasecmp(keyword, "read", kwlen) == 0) {
            /* 处理关键词 read */
            ...
        }
        else if (kwlen == 5 && strncasecmp(keyword, "write", kwlen) == 0) {
            /* 处理关键词 write */
            ...
        }
        else {
            /* 其他关键词 */
            ...
        }
    }
```

	
进一步提升代码的可维护性：

```c
#define strncasecmp2ltr(str, literal, len)      \
    ((len > (sizeof(literal "") - 1)) ? 1 :     \
        (len < (sizeof(literal "") - 1) ? -1 : strncasecmp(str, literal, len)))
...

    const char *keyword;
    size_t kwlen;
    for_each_keyword(options, total_len, keyword, kwlen) {
        if (strncasecmp2ltr(keyword, "read") == 0) {
            ...
        }
        else if (strncasecmp2ltr(keyword, "write") == 0) {
            ...
        }
        else {
            ...
        }
    }
```

	
### 方法二：遍历回调

树形结构的遍历接口：

```c
/** 遍历树形结构时的回调函数，传入节点和上下文；
    返回 true 时遍历继续，false 时终止遍历。 */
typedef bool (*pctree_node_for_each_cb)(pctree_node_t node, void *context);

/** 执行前序遍历，对每个节点调用指定的回调函数。 */
void pctree_node_pre_order_traversal(pctree_node_t root,
        pctree_node_for_each_cb for_each, void *context);

/** 执行中序遍历，对每个节点调用指定的回调函数。 */
void pctree_node_in_order_traversal(pctree_node_t root,
        pctree_node_for_each_cb for_each, void *context);

/** 执行后序遍历，对每个节点调用指定的回调函数。 */
void pctree_node_in_order_traversal(pctree_node_t root,
        pctree_node_for_each_cb for_each, void *context);

/** 执行层次遍历，对每个节点调用指定的回调函数。 */
void pctree_node_level_order_traversal(pctree_node_t root,
        pctree_node_for_each_cb for_each, void *context);
```

	
示例：

```c
struct my_node_data {
    int id;
};

/* 用于匹配节点的上下文结构体。 */
struct match_context {
    int             id;
    pctree_node_t   matched;
};

static bool match_needle(pctree_node_t node, void *context)
{
    struct my_node_data *node_data = pctree_node_get_user_data(node);
    struct match_context *ctxt = (struct match_context *)context;

    if (ctxt->id == node_data->id) {
        ctxt->matched = node;
        return false;
    }

    return true;
}

static pctree_node_t match_needle(pctree_node_t root, int needle)
{
    pctree_node_t found = NULL;
    struct match_context ctxt = { needle, NULL };
    pctree_node_pre_order_traversal(root, match_needle, &ctxt);
    if (ctxt.matched) {
        /* 找到匹配的节点。 */
        found = ctxt.matched;
    }
    else {
        /* 未找到节点。 */
    }

    return found;
}
```

	
### 方法三：迭代器

针对树形结构的迭代器接口：

```c
/* 声明迭代器类型，但不对外公开迭代器的内部结构。 */
struct pctree_iterator;

/* 创建一个前序迭代器。 */
struct pctree_iterator *
pctree_iterator_pre_order_create(pctree_node_t *root);

/* 创建一个中序迭代器。 */
struct pctree_iterator *
pctree_iterator_in_order_create(pctree_node_t *root);

/* 创建一个后序迭代器。 */
struct pctree_iterator *
pctree_iterator_post_order_create(pctree_node_t *root);

/* 创建一个层序迭代器。 */
struct pctree_iterator *
pctree_iterator_level_order_create(pctree_node_t *root);

/* 获取当前迭代器对应的节点。 */
pctree_node_t
pctree_iterator_entry(struct pctree_iterator *it);

/* 使迭代器前行一步，同时返回对应的节点。 */
pctree_node_t
pctree_iterator_next(struct pctree_iterator *it);

/* 使迭代器后退一步，同时返回对应的节点。 */
pctree_node_t
pctree_iterator_prev(struct pctree_iterator *it);

/* 销毁迭代器。 */
void
pctree_iterator_destroy(struct pctree_iterator *it);
```

	
示例：

```c
static pctree_node_t match_needle(pctree_node_t root, int needle)
{
    pctree_node_t found = NULL;
    struct pctree_iterator *it = pctree_iterator_pre_order_create(root);
    pctree_node_t node;

    while ((node = pctree_iterator_entry(it)) {
        struct my_node_data *node_data = pctree_node_get_user_data(node);
        if (node_data->id == needle) {
            /* 找到匹配的节点 */
            found = node;
            break;
        }

        pctree_iterator_next(it);
    }
    pctree_iterator_destroy(it);

    return found;
}
```

	
额外考虑：

- 安全迭代器：`foo_iterator_create_safe()` 或者 `foo_iterator_init_safe()`。
- 移除或删除接口：`foo_iterator_entry_remove()` 或 `foo_iterator_entry_delete()`。

		
## 模式八：接口的扩展和兼容性

经常会出现旧的接口设计考虑不周的情形：

1. 在标准C库中，大量早期接口的实现必须使用全局变量，从而导致这些接口不是线程安全的。
1. 某些接口的参数或者返回值之参数类型设计不当。
1. ……

	
### 扩展方法：预留扩展机制

```c
#include <sched.h>

struct clone_args {
    u64 flags;        /* Flags bit mask */
    u64 pidfd;        /* Where to store PID file descriptor
                         (pid_t *) */
    u64 child_tid;    /* Where to store child TID,
                         in child's memory (pid_t *) */
    u64 parent_tid;   /* Where to store child TID,
                         in parent's memory (int *) */
    u64 exit_signal;  /* Signal to deliver to parent on
                         child termination */
    u64 stack;        /* Pointer to lowest byte of stack */
    u64 stack_size;   /* Size of stack */
    u64 tls;          /* Location of new TLS */
    u64 set_tid;      /* Pointer to a pid_t array
                         (since Linux 5.5) */
    u64 set_tid_size; /* Number of elements in set_tid
                         (since Linux 5.5) */
    u64 cgroup;       /* File descriptor for target cgroup
                         of child (since Linux 5.7) */
};

long clone3(struct clone_args *cl_args, size_t size);
```

	
### 扩展方法：新旧接口共存

```c
#include <string.h>

char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);
```

	
### 扩展方法：旧接口只是新接口的绕转接口

```c
HWND CreateMainWindowEx2 (PMAINWINCREATE create_info, LINT id,
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
        unsigned int surf_flag, DWORD bkgnd_color,
        int compos_type, DWORD ct_arg);

static inline HWND CreateMainWindowEx (PMAINWINCREATE pCreateInfo,
                const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
                const char* window_name, const char* layer_name)
{
    return CreateMainWindowEx2 (pCreateInfo, 0L, werdr_name, we_attrs,
            ST_DEFAULT, 0xFFFFFFFFUL, CT_OPAQUE, 0);
}

static inline HWND CreateMainWindow (PMAINWINCREATE pCreateInfo)
{
    return CreateMainWindowEx (pCreateInfo, NULL, NULL, NULL, NULL);
}
```

	
需要二进制兼容性时：

```c
HWND CreateMainWindowEx (PMAINWINCREATE pCreateInfo,
                const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
                const char* window_name, const char* layer_name)
{
    return CreateMainWindowEx2 (pCreateInfo, 0L, werdr_name, we_attrs,
            ST_DEFAULT, 0xFFFFFFFFUL, CT_OPAQUE, 0);
}

HWND CreateMainWindow (PMAINWINCREATE pCreateInfo)
{
    return CreateMainWindowEx (pCreateInfo, NULL, NULL, NULL, NULL);
}
```

	
### 扩展方法：强制使用新接口，旧接口标记为废弃或移除

```c
gpointer g_memdup(gconstpointer mem, guint byte_size);
// since: 2.68
gpointer g_memdup2(gconstpointer mem, gsize byte_size);

// 使用时
#if GLIB_CHECK_VERSION(2, 68, 0)
    cache->cache[pos] = g_memdup2 (entry, sizeof (*entry));
#else
    cache->cache[pos] = g_memdup (entry, sizeof (*entry));
#endif
```

	
### 扩展接口需要考虑的因素

1. 新增接口而非修改原有接口的语义
1. 二进制兼容还是源代码兼容？
   - 宏或者内联函数实现接口的向后兼容性，无法保证二进制兼容。

		
## 作业

		
## 模式七：面向对象

- 面向对象最本质的特征：派生、重载和多态。
- 早期的 C++ 编译器是将 C++ 代码翻译为 C 代码然后再编译成二进制代码的。
- C++ 的 `class` 会被翻译为两个 C 的数据结构：
   1. 一个数据结构定义该对象的属性（普通结构成员）。
   1. 另一个定义这个类的内部数据、类方法（函数指针）以及可重载的虚函数；这个数据结构对所有这个类的对象公用。
- `namespace`、`class` 等，在生成的 C 函数名称之前增加前缀。

	
### mGNCS 实现面向对象的接口

比如一个基类 `mObject`，一个派生类 `mComponent`：

```c
#define ClassType(className)    className##Class

typedef struct _mObjectClass mObjectClass;
typedef struct _mObject mObject;

#define mObjectClassHeader(clss, superCls) \
    PClassConstructor classConstructor; \
    ClassType(superCls) * super; \
    const char* typeName; \
    unsigned short objSize; \
    unsigned short intfOffset; /** the interface offset */ \
    void (*construct)(clss *self, void* addData); \
    void (*destruct)(clss *self); \
    unsigned int (*ref)(clss*); \
    unsigned int (*unref)(clss*);

struct _mObjectClass {
    mObjectClassHeader(mObject, mObject)
};

extern mObjectClass g_stmObjectCls;

#define mObjectHeader(clss) \
    ClassType(clss) * _class; \
    unsigned int objRefCount; \
    unsigned int objStatus;

struct _mObject {
    mObjectHeader(mObject)
};


mObject * newObject(mObjectClass *_class);
static inline mObject * ncsNewObject(mObjectClass *_class, void* addData) {
    mObject * obj = newObject(_class);
    if(!obj)
        return NULL;

    _class->construct(obj, addData);
    return obj;
}

#define NEWEX(classType, addData)  \
    (classType*)(void*)ncsNewObject((mObjectClass*)(void *)(&Class(classType)), (DWORD)(addData))
#define NEW(classType)  NEWEX(classType, 0)

void deleteObject(mObject *obj);
#define DELETE(obj)   deleteObject((mObject*)(obj))

typedef struct _mComponentClass mComponentClass;
typedef struct _mComponent mComponent;

#define mComponentClassHeader(clss, superCls) \
    mObjectClassHeader(clss, superCls) \
    const char* className; \
    clss * (*createComponent)(mComponent* parent, ClassType(clss)*, void *addValue); \
    BOOL (*setProperty)(clss *self, int id, void *value); \
    void *(*getProperty)(clss *self, int id); \
    int (*setId)(clss *self, int id); \
    int (*getId)(clss *self); \
    mComponent* (*getChild)(clss* self, int id);  \
    void        (*removeChild)(clss* self, mComponent* child);

struct _mComponentClass {
    mComponentClassHeader(mComponent, mObject)
};

extern mComponentClass g_stmComponentCls; //Class(mComponent);

#define mComponentHeader(clss) \
    mObjectHeader(clss) \
    NCS_EVENT_HANDLER_NODE * event_handlers; \
    mSpecificDataNode* specificHead;

struct _mComponent {
    mComponentHeader(mComponent)
};
```

	
### C 语言实现面向对象接口的特点

1. 两个数据结构用于操作集及对象数据的解耦
1. 充斥着大量的宏和指针运算
1. 派生容易实现，虚函数重载和多态的实现和使用比较别扭

	
### 参考实现

mGNCS 文档：<http://wiki.minigui.com/twiki/bin/view/Products/MStudioMGNCSV1dot0PG>
mGNCS 仓库：<https://gitlab.fmsoft.cn/VincentWei/mgncs>
