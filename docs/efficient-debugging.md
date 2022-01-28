## 第十二讲：高效调试

- 时间：2022 年 1 月 29 日（周六）17：30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 高效调试的基本原则
1. C 程序常见错误分类
1. 内存使用错误
1. 善用调试工具

		
## 高效调试的基本原则

1. 多读代码、读懂代码
1. 不要慌，不要怀疑一切，尤其不要轻易怀疑编译工具链
1. 快速找到第一现场，理清现象和缺陷的内在关联
   - 复原现场，形成完整的证据链
   - 知其然，更要知其所以然
1. 避免使用低效调试手段

		
## C 程序常见错误分类

1. 编译、链接错误（5%）
1. 算法或逻辑错误（20%）
1. 内存使用错误（70%）
1. 疑难杂症（5%）

	
### 编译、链接错误

1. 编译错误
   - 始终解决第一个错误
   - 正确理解编译器给出的错误描述
1. 链接错误：未找到某某符号
   - 正确的函数库（包括版本）和库搜索路径
   - 静态链接时，根据函数库依赖关系，调整函数库顺序

	
### 算法或逻辑错误

- 主要靠读代码和单元测试

	
### 内存使用错误

1. 堆使用错误
1. 栈使用错误

	
### 疑难杂症

1. 命名污染，导致调用错误的函数或者使用错误的全局变量
1. 整数溢出
1. 异步数据访问
   - Unix 系统中的信号处理
   - 多线程程序中出现竞态（race-condition）

		
## 内存使用错误

	
### 堆使用错误

1) 超范围访问
2) 使用无效地址（空指针、野指针或者已释放的指针）
3) 内存泄露（忘记释放）
4) 两次释放
5) 释放非分配地址

	
### 堆管理算法对错误现象的影响

1. 不是所有的堆使用错误都会出现错误或者立即出现错误

```c
#define CSTR_HELLO  "hello, world!"

static void access_out_of_range(unsigned int range)
{
    const char *hello = CSTR_HELLO;
    char *buff;

    buff = malloc(sizeof(char) * 4);

    if (range > sizeof(CSTR_HELLO)) {
        printf("Going to memset %d bytes\n", range);
        memset(buff, 0, range);
    }
    else {
        for (int i = 0; i < range; i++) {
            buff[i] = hello[i];
        }

        for (int i = 0; i < range; i++) {
            putchar(buff[i]);
        }
        puts("");
    }

    free(buff);
}

int main(void)
{
    unsigned int range = 8;

    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
hello, w
Going to memset 16 bytes
Going to memset 32 bytes
Going to memset 64 bytes
Going to memset 128 bytes
Going to memset 256 bytes
Going to memset 512 bytes
Going to memset 1024 bytes
Going to memset 2048 bytes
Going to memset 4096 bytes
Going to memset 8192 bytes
Going to memset 16384 bytes
Going to memset 32768 bytes
Going to memset 65536 bytes
Going to memset 131072 bytes
Going to memset 262144 bytes
Segmentation fault (core dumped)
```

	
### 栈使用错误

1) 缓冲区溢出
2) 栈溢出

	
### 可变参数的副作用

1. 可变参数（如格式化输入输出函数）可能会消除栈使用错误

		
## 常用调试工具和技巧

1. gdb：程序挂在哪儿了？
1. efence：程序是否有堆使用错误？
1. valgrind：是否存在内存泄露？
1. 打桩

	
### gdb

	
### efence

	
### valgrind

	
### 打桩

		
## Q & A

