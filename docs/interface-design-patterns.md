## 第五讲：接口设计模式

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

1. 完备（completeness）：完整性
1. 自洽（self-consistency）：符合逻辑

	
## 不好的例子

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

// 设置当前会话的基础 URL，并返回主机名（FQDN）。
const char* pcfetcher_set_base_url(const char* base_url);

// 设置 COOKIE
void pcfetcher_cookie_set(const char* host,
        const char* path, const char* name, const char* content, bool secure);

// 获取 COOKIE
const char* pcfetcher_cookie_get(const char* host,
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

	
### 范例二：HVML 解释器 PurC 的变体接口

		
## 模式二：抽象算法

1. 围绕抽象算法设计接口
1. 算法不依赖于具体的数据类型
1. 算法不依赖于具体的数据存储方式

	
### 范例一：STDC 的 `qsort` 函数

	
### 范例二：MiniGUI 的曲线生成器函数

		
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

