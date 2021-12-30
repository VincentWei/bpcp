## 第八讲：动态加载模块

- 时间：2021 年 12 月 30 日 18:30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 可加载模块的应用场景
1. 再论机制和策略
1. 可加载模块的底层机制
1. 一般实现套路
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

1. 调用 `dlopen` 装载指定的共享库。
1. 调用 `dlsym` 获得特定（实现约定的）符号，作为初始化函数。
1. 调用初始化函数并返回操作集。
1. 之后，跟正常的子驱动程序一样使用。
1. 不需要的时候，调用操作集中的清理或终止方法。
1. 调用 `dlclose` 卸载动态模块。

		
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
   - [i915驱动实现](https://github.com/FMSoftCN/hidrmdrivers/blob/master/src/intel/intel-i915-driver.c)
- [Mesa3D 应用层显卡驱动](https://www.mesa3d.org)
- [Linux 内核模块](https://www.kernel.org/doc/html/latest/driver-api/index.html)

		
## 下一讲预告

- 主题：状态机
- 地点：微信视频号“考鼎录”直播间
- 时间：2022 年 1 月 6 日 18:30

		
## Q & A

