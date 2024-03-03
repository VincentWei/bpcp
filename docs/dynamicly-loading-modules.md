## 第九章：动态加载模块

1. 可加载模块的应用场景
1. 再论机制和策略
1. 可加载模块的底层机制
1. 一般实现套路
1. 案例：使用动态加载模块支持新的图片格式
1. 案例：MiniGUI 5.0 的合成器

		
## 可加载模块的应用场景

1. 可加载模块是子驱动程序模式的一种自然扩展
1. 为应对未来不可预知的扩展
1. 将某些功能的实现交给第三方独立模块实现
1. 软件模块间解耦的利器

		
## 再论机制和策略

- 机制：需要提供什么功能
- 策略：如何使用这些功能
- 机制和策略是相对的
  - 对 STDIO 来讲，带缓冲区的 FILE 是使用策略，文件描述符是一种实现机制
  - 对内核进程来讲，文件描述符是策略，字符设备是一种实现机制，文件系统是另外一种实现机制
- 那么，我们如何准确划分机制和策略？

		
## 软件栈和模块间解耦

1. 操作系统是堆叠的软件栈，越往上抽象层次越高
1. 可加载模块可以在不同的层面实现
   - 内核
   - 中间件
   - 编程语言
1. 可加载模块的重要设计原则：
   - 认清模块在软件栈中的位置
   - 向下依赖，不要向上依赖
   - 避免同级依赖

	
### Android 软件栈

<img class="fragment" src="assets/figure-9.1.svg" height="600">

	
### MiniGUI 架构及关键模块

<img class="fragment" src="assets/figure-9.2.svg" height="600">

		
## 可加载模块的底层机制

1) POSIX 系统：

```c
#include <dlfcn.h>

void *dlopen(const char *filename, int flags);
void *dlsym(void *handle, const char *symbol);
int dlclose(void *handle);
```

2) Windows：

```c
HMODULE LoadLibraryA(LPCSTR lpLibFileName);
FARPROC GetProcAddress(HMODULE hModule, LPCSTR  lpProcName);
BOOL FreeLibrary(HMODULE hLibModule);
```

	
### Linux 共享库的版本

以 CURL 库为例：

```
-rw-r--r-- 1 root root  1192212 9月  10 22:28 libcurl.a
lrwxrwxrwx 1 root root       19 9月  10 22:28 libcurl-gnutls.so.3 -> libcurl-gnutls.so.4
lrwxrwxrwx 1 root root       23 9月  10 22:28 libcurl-gnutls.so.4 -> libcurl-gnutls.so.4.6.0
-rw-r--r-- 1 root root   580216 9月  10 22:28 libcurl-gnutls.so.4.6.0
-rw-r--r-- 1 root root      953 9月  10 22:28 libcurl.la
lrwxrwxrwx 1 root root       16 9月  10 22:28 libcurl.so -> libcurl.so.4.6.0
lrwxrwxrwx 1 root root       16 9月  10 22:28 libcurl.so.4 -> libcurl.so.4.6.0
-rw-r--r-- 1 root root   588424 9月  10 22:28 libcurl.so.4.6.0
```

	
### 装载不同共享库名称的区别

```c
dlopen('libcurl-gnutls.so.3', RTLD_LAZY);
dlopen('libcurl-gnutls.so.4', RTLD_LAZY);
dlopen('libcurl-gnutls.so.4.6.0', RTLD_LAZY);
```

	
### 共享库搜索路径

1. 若调用这个 `dlopen` 的可执行程序是 ELF 格式，则查看 `DT_RPATH` 标记指定的共享库路径。
1. `LD_LIBRARY_PATH` 环境变量定义的共享库搜索路径。
1. 若调用这个 `dlopen` 的可执行程序是 ELF 格式，则查看 `DT_RUNPATH` 标记指定的共享库路径。
1. `/etc/ld.so.cache` 维护的搜索路径（由 `ldconfig` 维护，来自 `/etc/ld.so.conf` 文件）。
1. 最后搜索 `/lib` 和 `/usr/lib` 路径。

		
## 一般实现套路

	
### 加载

1) 调用 `dlopen()` 装载指定的共享库。
2) 调用 `dlsym()` 获得约定的版本号符号，判断版本号并执行可能的验证签名工作。
3) 调用 `dlsym()` 获得约定的初始化函数符号，作为模块的初始化函数。
4) 调用模块的初始化函数并返回操作集；或调用模块的初始化函数，由初始化函数调用系统提供的注册接口完成子驱动程序的注册。
5) 此时系统中便多了一个或者若干个新的子驱动程序，跟内建到系统中的子驱动程序一样使用。

	
### 卸载

6) 当不再需要这些子驱动程序时，调用系统提供的移除子驱动程序的接口，从系统中移除对应的子驱动程序。
7) 然后调用 `dlsym()` 获得约定的清理或终止函数符号，并调用该函数执行模块的清理工作。
8) 调用 `dlclose()` 卸载动态加载的模块（共享库）。

		
## 案例：使用可加载模块支持新的图片格式

	
- API

```c
BOOL RegisterImageType (const char *ext,
            void* (*init) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            int (*load) (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp,
            CB_ONE_SCANLINE cb, void* context),
            void (*cleanup) (void* init_info),
            int (*save) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            BOOL (*check) (MG_RWops* fp));

BOOL RevokeImageType(const char *ext);
```

	
- 使用可动态改变的内部数据结构管理子驱动程序

```c
static struct _IMAGE_TYPE_INFO image_types[] = {
    { "bmp", bmp_init, bmp_load, bmp_cleanup },
    { "png", png_init, png_load, png_cleanup },
    { "jpg", jpg_init, jpg_load, jpg_cleanup },
    { "jpeg", jpg_init, jpg_load, jpg_cleanup },
    { "gif", gif_init, gif_load, gif_cleanup },
};

static pchas_table *hased_images;

int init_image_sub_drivers(void)
{
    assert(hashed_images);
    hashed_images = pchash_kstr_table_new(8);
    if (hashed_images == NULL)
        goto failed;

    for (size_t i = 0; i < sizeof(image_types)/sizeof(image_types[0]); i++) {
        if (pchash_table_insert(hashed_images, image_types[i].ext, image_types + i))
            goto failed;
    }

    return 0;

failed:
    if (hashed_images) {
        pchash_table_delete(hashed_images);
        hashed_images = NULL;
    }

    return ENOMEM;
}

static struct _IMAGE_TYPE_INFO *find_img_sub_driver(const char *ext)
{
    struct _IMAGE_TYPE_INFO *type;

    if (pchash_table_lookup(hashed_images, ext, &type))
        return type;

    return NULL;
}
```

	
- 约定的符号

遵照上个小节提到的可加载模块的实现要点，我们假定使用如下约定的符号：

- `__ex_image_type_subdrv_version`：用于定义子驱动程序操作集接口的版本编号，`int` 型。
- `__ex_image_type_subdrv_init`：用于模块的初始化函数，该函数向系统中注册新的图片类型。
- `__ex_image_type_subdrv_term`：用于模块的终止函数，该函数从系统中注销一个图片类型。该符号是可选的，因为在大部分情况下，解码特定图片格式的代码无需分配全局资源。

	
- 用于 WebP 的子驱动程序接口及其实现

```c
/* 在 MiniGUI 头文件中定义 */
#define MG_IMAGE_TYPE_SDI_VERSION   3

...

static IMAGE_TYPE_CTXT *webp_init(FILE* fp, MYBITMAP *my_bmp, RGB *pal)
{
    ...
}

static int webp_load(FILE* fp, IMAGE_TYPE_CTXT *ctxt, MYBITMAP *my_bmp,
    CB_ONE_SCANLINE cb, void *cb_ctxt)
{
    ...
}

static void webp_cleanup(IMAGE_TYPE_CTXT *ctxt)
{
    ...
}

static BOOL webp_check(FILE* fp)
{
    ...
}

int __ex_image_type_subdrv_version = MG_IMAGE_TYPE_SDI_VERSION;

BOOL __ex_image_type_subdrv_init(void)
{
    return RegisterImageType("webp", webp_init, webp_load, webp_cleanup,
            webp_save, webp_check);
}

BOOL __ex_image_type_subdrv_term(void)
{
    return RevokeImageType("webp");
}
```

	
- 装载动态模块的代码

```c
BOOL load_image_type_sub_driver(const char *module)
{
    void *dl_handle = NULL;
    char *error;

    /* 清除已有的装载错误。 */
    dlerror();

    dl_handle = dlopen(module, RTLD_LAZY);
    if (!dl_handle) {
        /* 装载失败。 */
        goto failed;
    }

    int *version = dlsym(dl_handle, "__ex_image_type_subdrv_version");
    if (version == NULL || *version != MG_IMAGE_TYPE_SDI_VERSION) {
        /* 找不到版本编号对应的符号或者和当前版本不匹配。 */
        goto failed;
    }

    BOOL (*init)(void);
    init = dlsym(dl_handle, "__ex_image_type_subdrv_init");
    if (init == NULL) {
        /* 找不到模块的初始化函数。 */
        goto failed;
    }

    if (!init()) {
        /* 初始化失败。 */
        goto failed;
    }

    return TRUE;

failed:
    error = dlerror ();
    if (error)
        _WRN_PRINTF ("dlxxx error: %s\n", error);

    if (dl_handle)
        dlclose(dl_handle);

    return FALSE;
}
```

		
## 案例：MiniGUI 5.0 的窗口合成器

1. 窗口合成器的概念
1. [演示](https://player.youku.com/embed/XNTgyNTkxOTIwNA==)

	
### 操作集

```c
/**
 * The struct type represents the context of a compositor.
 * The concrete struct should be defined by the implementation of the compositor.
 */
struct _CompositorCtxt;
typedef struct _CompositorCtxt CompositorCtxt;

/**
 * The struct type defines the operations for a compositor.
 */
typedef struct _CompositorOps {
    /**
     * This operation initializes the compositor
     * and returns the compositing context.
     */
    CompositorCtxt* (*initialize) (const char* name);

    /**
     * This operation destroies the compositing context
     * and terminates the compositor.
     */
    void (*terminate) (CompositorCtxt* ctxt);

    ...

    /**
     * This operation will composite multiple layers on the screen at the same
     * time by using the combining paramemters.
     */
    unsigned int (*composite_layers) (CompositorCtxt* ctxt, MG_Layer* layers[],
            int nr_layers, void* combine_param);

    /**
     * This operation will be called when transiting to a new topmost layer.
     * The compositor can play an animation to transit from the current
     * topmost layer to the new topmost layer.
     */
    void (*transit_to_layer) (CompositorCtxt* ctxt, MG_Layer* to_layer);

} CompositorOps;
```

	
### 装载默认合成器

```c
#define MAX_NR_COMPOSITORS      8
#define LEN_COMPOSITOR_NAME     15

extern CompositorOps __mg_fallback_compositor;

static struct _compositors {
    char name [LEN_COMPOSITOR_NAME + 1];
    const CompositorOps* ops;
} compositors [MAX_NR_COMPOSITORS] = {
    { "fallback", &__mg_fallback_compositor },
};

static void* dl_handle;

static const CompositorOps* load_default_compositor (void)
{
    const char* filename = NULL;
    char buff [LEN_SO_NAME + 1];
    const CompositorOps* (*ex_compsor_get) (const char*,
            const CompositorOps*, int*);
    char* error;
    const CompositorOps* ops;
    int version = 0;

    filename = getenv ("MG_DEF_COMPOSITOR_SO");
    if (filename == NULL) {
        memset (buff, 0, sizeof (buff));
        if (GetMgEtcValue ("compositing_schema", "def_compositor_so",
                buff, LEN_SO_NAME) < 0)
            return NULL;

        filename = buff;
    }

    dl_handle = dlopen (filename, RTLD_LAZY);
    if (!dl_handle) {
        _WRN_PRINTF ("Failed to open specified shared library for the default compositor: %s (%s)\n",
                filename, dlerror());
        return NULL;
    }

    dlerror();    /* Clear any existing error */
    ex_compsor_get = dlsym (dl_handle, "__ex_compositor_get");
    error = dlerror ();
    if (error) {
        _WRN_PRINTF ("Failed to get symbol: %s\n", error);
        dlclose (dl_handle);
        return NULL;
    }

    ops = ex_compsor_get (COMPSOR_NAME_DEFAULT,
        &__mg_fallback_compositor, &version);

    if (ops == NULL || version != COMPSOR_OPS_VERSION)
        return NULL;

    return ops;
}

BOOL mg_InitCompositor (void)
{
    const char* name = COMPSOR_NAME_FALLBACK;
    const CompositorOps* ops;
    CompositorCtxt* ctxt = NULL;

    ops = load_default_compositor ();
    if (ops && ServerRegisterCompositor (COMPSOR_NAME_DEFAULT, ops)) {
        name = COMPSOR_NAME_DEFAULT;
    }

    ServerSelectCompositor (name, &ctxt);
    return (ctxt != NULL);
}

void mg_TerminateCompositor (void)
{
    CompositorCtxt* ctxt = NULL;

    // Select fallback compositor and terminate the fallback compositor.
    ServerSelectCompositor (COMPSOR_NAME_FALLBACK, &ctxt);
    if (ctxt) {
        purge_all_znodes_private_data (&__mg_fallback_compositor, ctxt);
        __mg_fallback_compositor.terminate (ctxt);
    }

    if (dl_handle)
        dlclose (dl_handle);
}
```

	
### 应用接口

```c
const CompositorOps* GUIAPI ServerGetCompositorOps (const char* name);

BOOL GUIAPI ServerRegisterCompositor (const char* name,
        const CompositorOps* ops);
BOOL GUIAPI ServerUnregisterCompositor (const char* name);

const CompositorOps* GUIAPI ServerSelectCompositor (
            const char* name, CompositorCtxt** ctxt);

const char* GUIAPI ServerGetCurrentCompositor (
        const CompositorOps** ops, CompositorCtxt** ctxt);
```

	
### 派生一个自己的合成器

```c
CompositorOps my_compor_ops;
CompositorCtxt* my_ctxt;

unsigned int my_composite_layers (CompositorCtxt* ctxt, MG_Layer* layers[],
            int nr_layers, void* combine_param)
{
    ...
}

void my_transit_to_layer (CompositorCtxt* ctxt, MG_Layer* to_layer)
{
    ...
}

CompositorOps* ops;

ops = ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
memcpy (&my_compor_ops, ops, sizeof(CompositorOps));

my_compor_ops.composite_layers = my_composite_layers;
my_compor_ops.transit_to_layer = my_transit_to_layer;

ServerRegisterCompositor ("my_compor", &my_compor_ops);

ServerSelectCompositor ("my_compor", &my_ctxt);
```

		
## 更多案例

- MiniGUI 的 DRM 驱动程序
   - [管理代码](https://github.com/VincentWei/minigui/blob/master/src/newgal/drm/drmvideo.c#L1041)
   - [i915驱动实现](https://github.com/HybridOS2/hidrmdrivers/blob/master/src/intel/intel-i915-driver.c)
- [Mesa3D 应用层显卡驱动](https://www.mesa3d.org)
- [Linux 内核模块](https://www.kernel.org/doc/html/latest/driver-api/index.html)

		
## 作业

1. 增强第八章的作业，使之支持以动态加载模块的方式自定义的输出目标，该模块会将写入的数据进行 MD5 散列，实际写入输出目标的内容为 MD5 散列值。

