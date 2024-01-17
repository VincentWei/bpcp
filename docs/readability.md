## 第一章：提高代码可读性

- 代码可读性的衡量标准
- 怀代码的共同点
- 编码风格到底规定了啥东西？
- 和编码风格相关的争议
- 命名的艺术

		
## 代码可读性的衡量标准

* 初级：能编译通过即可，这表明计算机能读懂你的代码。
* 中级：三个月之后，你自己还能看懂。
* 高级：接手你工作的其他码农能看得懂，且改得动。
* 神级：不需要修改和维护，可以运行“万年”的。

		
## 为什么有些代码看了就想吐？

```c
typedef struct Linklist{
    const char * elem;
    struct Linklist * next;
}linklist;

//初始化链表的函数
linklist * initlinklist();

const char * titles[]={"第1章 提高代码可读性","第2章 用好写好头文件",
"第3章 消除所有编译警告","第4章 常量的定义和使用",
"第5章 善用构建系统生成器"};

int main() {
    // 使用章节标题初始化链表
    printf("初始化链表为：\n");
    linklist *p=initlinklist();
    display(p);
    return 0;
}
linklist * initlinklist(){
    linklist * p=NULL;  //创建头指针
    linklist * temp = ( linklist*)malloc(sizeof(linklist));

    // 先初始化首元节点
    temp->elem = titles[0];
    temp->next = NULL;
    p = temp; // 头指针指向首元节点

    for (int i=1; i<5; i++) {
        linklist *a=(linklist*)malloc(sizeof(linklist));
        a->elem=titles[i];
        a->next=NULL;
        temp->next=a;
        temp=temp->next;
}
return p;
}
```

		
## 坏代码的共同点

不好的编码风格

- 排版混乱
- 书写拥挤
- 混用不同的编码风格

		
## 坏代码的共同点

不好的命名

- 不正确的术语
- 不符合习惯
- 错误的时态
- 使用拼音
- 含有中文等特殊字符的文件名

		
## 坏代码的共同点

不好的注释

- 太多或太少
- 使用中文
- 注释太花哨

		
## 实例：随处可见的坏代码

		
## 编码风格到底规定什么？

1. 排版规则
   - 缩进、空格、换行、大括号位置等
1. 命名规则
   - K & R 规则：`this_is_an_integer`
   - 匈牙利规则：`iThisIsAnInteger`
   - 其他：比如宏、枚举常量、全局变量等的命名规则
1. 其他规则
   - 自定义类型的使用
   - 条件编译的写法
   - 注释的写法
   - 经验写法

		
## 实例：Linux 内核的编码风格

[点这里](https://www.kernel.org/doc/html/latest/process/coding-style.html)

		
## 有关C编码风格的常见争议

80 列红线

- 最佳实践：务必守好 80 列红线
   - 防止过多缩进嵌套，强制改进代码
   - 在大屏幕上，方便同时查看多个源文件文件。

		
## 有关C编码风格的常见争议

空格

- 最佳实践：Linux 内核编码风格
   - 单目运算符、`++`、`--`不加空格：
```c
    int *p = (int *)&a;
    p++;
```
	
- 最佳实践：Linux 内核编码风格
   - 函数名称，包括可按函数调用的关键词之后，不要加空格：
```c
    call_me(sizeof(int), MAX(x, y));
```
	
- 最佳实践：Linux 内核编码风格
   - 不要在行尾加空格。
   - 其他情形，如双目或多目运算符前后、关键词之后都要加空格：
```c
   if (a && b) {
       ...
   }
```

		
## 有关C编码风格的常见争议

指针的星号位置

- 最佳实践：Linux 内核编码风格

```c
    void *get_context(struct node *node);
```

		
## 有关C编码风格的常见争议

下划线前缀

- 最佳实践：
   - 仅针对 extern 变量或者函数使用下划线前缀：
```c
    extern size_t __total_mem_use;
```
   - 作用：主要用于防止命名污染

		
## 有关C编码风格的常见争议

何时使用 typedef

- 最佳实践：无
- 我的实践：
   - 仅在函数库的接口定义中使用 typedef
   - 仅对结构的指针使用 typedef，并使用 `_t` 后缀
   - 对枚举或者结构的类型定义名称使用驼峰命名法

		
## 有关C编码风格的常见争议

命名规则

- 最佳实践：倾向 K & R 规则，避免使用匈牙利命名法如 spName
- 我的实践：除接口外，内部使用 K & R 命名规则

		
## 有关C编码风格的常见争议

注释

- 最佳实践
   - 避免使用 C++ 注释： //
   - 巧用 XXX、FIXME、TODO 等短语
   - 使用 Doxygen 格式来撰写 API 文档说明

		
## 有关C编码风格的常见争议

到底用不用 `goto`？

- 最佳实践
   - `goto` 仅在处理函数的错误返回时使用，用于将函数的返回统一在函数末尾处理。

```c
struct pchash_table *pchash_table_new(...)
{
    struct pchash_table *t = NULL;

    ...
    t = (struct pchash_table *)calloc(1, sizeof(struct pchash_table));
    if (!t)
        goto failed;

    ...
    t->table = (struct pchash_entry *)calloc(size, sizeof(struct pchash_entry));
    if (!t->table) {
        goto failed;
    }

    ...

    if (do_other_initialization(t)) {
        goto failed;
    }

    return t;

failed:
    if (t) {
        if (t->table)
            free(t->table);
        free(t);
    }

    return NULL;
}
```

		
## 有关C编码风格的常见争议

`do-while(0)`

```c
struct pchash_table *pchash_table_new(...)
{
    struct pchash_table *t = NULL;

    do {
        t = (struct pchash_table *)calloc(1, sizeof(struct pchash_table));
        if (!t)
            break;

        ...
        t->table = (struct pchash_entry *)calloc(size,
                sizeof(struct pchash_entry));
        if (!t->table) {
            break;
        }

        ...

        if (do_other_initialization(t)) {
            break;
        }

        return t;
    } while (0);

    if (t) {
        if (t->table)
            free(t->table);
        free(t);
    }

    return NULL;
}
```

		
## 命名的艺术

一般性原则

- 使用正确的英文术语及其简写  
node, child, sibling, parent, root, first, last, next, previous, …
- 避免使用万金油名称  
item、data

		
## 命名的艺术

一般性原则

- 正确使用时态  
`linked_list`
- 正确使用单数、复数形式  
nodes, children


		
## 命名的艺术

一般性原则

- 使用喜闻乐见的缩写  
nr, sz, dbl, tri, len, max, min, buf, ver, id,  prev, tmp, param, arg, argc, argv, conn, ctxt, err, … 

		
## 命名的艺术

局部变量

- 简洁：
   - 循环：i, j, k；数量：n；长度：len； 
   - 尺寸：sz；指针：p；
   - 临时变量：tmp/temp；临时缓冲区：buf；

		
## 命名的艺术

函数及全局变量的名称

- 接口：
```c
extern "C" const char *hbdbus_get_error_message(int errcode);
```
- 文件内：
```c
static void set_errcode(int errcode)
{
    ...
}
```
	
- 模块间：
```c
bool _websocket_get_current_packet(void)
    __attribute__((visibility("hidden")));
```

		
## 实例

经典的循环链表结构体及其接口

```c
/*
 * @file list.h
 * @date 2021/07/05
 * @brief The interfaces for loop list.
 *
 * Copyright (c) 2021 FMSoft <https://www.fmsoft.cn>
 * Copyright (c) 2011 Felix Fietkau <nbd@openwrt.org>
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef PURC_PRIVATE_LIST_H
#define PURC_PRIVATE_LIST_H

#include <stddef.h>
#include <stdbool.h>

#ifndef container_of
#define container_of(ptr, type, member)                                 \
    ({                                                                  \
        const __typeof__(((type *) NULL)->member) *__mptr = (ptr);      \
        (type *) (void*) (((char *)__mptr) - offsetof(type, member));   \
    })
#endif

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#undef LIST_HEAD
#define LIST_HEAD(name)    struct list_head name = LIST_HEAD_INIT(name)

static inline void
list_head_init(struct list_head *list)
{
    list->next = list->prev = list;
}

#define INIT_LIST_HEAD  list_head_init

static inline bool
list_empty(const struct list_head *head)
{
    return (head->next == head);
}

static inline bool
list_is_first(const struct list_head *list,
          const struct list_head *head)
{
    return list->prev == head;
}

static inline bool
list_is_last(const struct list_head *list,
         const struct list_head *head)
{
    return list->next == head;
}

static inline void
_list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

static inline void
list_del(struct list_head *entry)
{
    _list_del(entry);
    entry->next = entry->prev = NULL;
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
list_del_init(struct list_head *entry)
{
    _list_del(entry);
    INIT_LIST_HEAD(entry);
}

#define list_entry(ptr, type, field)    container_of(ptr, type, field)
#define list_first_entry(ptr, type, field)    list_entry((ptr)->next, type, field)
#define list_last_entry(ptr, type, field)    list_entry((ptr)->prev, type, field)

#define list_for_each(p, head)                               \
    for (p = (head)->next; p != (head); p = p->next)

#define list_for_each_safe(p, n, head)                       \
    for (p = (head)->next, n = p->next; p != (head); p = n, n = p->next)

#define list_for_each_entry(p, h, field)                                        \
    for (p = list_first_entry(h, __typeof__(*p), field); &p->field != (h);      \
        p = list_entry(p->field.next, __typeof__(*p), field))

#define list_for_each_entry_safe(p, n, h, field)                                \
    for (p = list_first_entry(h, __typeof__(*p), field),                        \
        n = list_entry(p->field.next, __typeof__(*p), field); &p->field != (h); \
        p = n, n = list_entry(n->field.next, __typeof__(*n), field))

#define list_for_each_entry_reverse(p, h, field)                             \
    for (p = list_last_entry(h, __typeof__(*p), field); &p->field != (h);       \
        p = list_entry(p->field.prev, __typeof__(*p), field))

#define list_for_each_entry_reverse_safe(p, n, h, field)                  \
    for (p = list_last_entry(h, __typeof__(*p), field),                      \
         n = list_entry(p->field.prev, __typeof__(*p), field);               \
         &p->field != (h);                                                   \
        p = n, n = list_entry(p->field.prev, __typeof__(*p), field))

#define list_for_each_prev(p, h) for (p = (h)->prev; p != (h); p = p->prev)
#define list_for_each_prev_safe(p, n, h) for (p = (h)->prev, n = p->prev; p != (h); p = n, n = p->prev)

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

static inline void
list_move(struct list_head *list, struct list_head *head)
{
    _list_del(list);
    list_add(list, head);
}

static inline void
list_move_tail(struct list_head *entry, struct list_head *head)
{
    _list_del(entry);
    list_add_tail(entry, head);
}

static inline void
_list_splice(const struct list_head *list, struct list_head *prev,
    struct list_head *next)
{
    struct list_head *first;
    struct list_head *last;

    if (list_empty(list))
        return;

    first = list->next;
    last = list->prev;
    first->prev = prev;
    prev->next = first;
    last->next = next;
    next->prev = last;
}

static inline void
list_splice(const struct list_head *list, struct list_head *head)
{
    _list_splice(list, head, head->next);
}

static inline void
list_splice_tail(struct list_head *list, struct list_head *head)
{
    _list_splice(list, head->prev, head);
}

static inline void
list_splice_init(struct list_head *list, struct list_head *head)
{
    _list_splice(list, head, head->next);
    INIT_LIST_HEAD(list);
}

static inline void
list_splice_tail_init(struct list_head *list, struct list_head *head)
{
    _list_splice(list, head->prev, head);
    INIT_LIST_HEAD(list);
}

static inline int
list_swap(struct list_head *l, struct list_head *r)
{
    if (l == r)
        return 0;

    struct list_head *l_prev = l->prev;
    struct list_head *l_next = l->next;
    struct list_head *r_prev = r->prev;
    struct list_head *r_next = r->next;
    if (l_prev == r) {
        list_del(r);
        _list_add(r, l, l_next);
        return 0;
    }
    if (l_next == r) {
        list_del(l);
        _list_add(l, r, r_next);
        return 0;
    }
    list_del(l);
    _list_add(l, r_prev, r);
    list_del(r);
    _list_add(r, l_prev, l_next);
    return 0;
}

static inline int
list_swap_list(struct list_head *lh, struct list_head *rh)
{
    if (lh == rh)
        return 0;

    if (!list_empty(lh)) {
        struct list_head *l_head = lh->next;
        struct list_head *l_tail = lh->prev;
        INIT_LIST_HEAD(lh);
        if (!list_empty(rh)) {
            struct list_head *r_head = rh->next;
            struct list_head *r_tail = rh->prev;
            INIT_LIST_HEAD(rh);

            _list_add(rh, l_tail, l_head);
            _list_add(lh, r_tail, r_head);
        }
        else {
            _list_add(rh, l_tail, l_head);
        }
    }
    else {
        if (!list_empty(rh)) {
            struct list_head *r_head = rh->next;
            struct list_head *r_tail = rh->prev;
            list_del_init(rh);
            _list_add(lh, r_head, r_tail);
        }
    }

    return 0;
}

#endif /* PURC_PRIVATE_LIST_H */
```

		
## 作业

1. 参照 Linux 内核编码风格整理 bin2c.c 源文件。

		
## Q & A

