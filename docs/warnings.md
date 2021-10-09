## 第三讲：清除所有编译警告

- 时间：9 月 30 日 20:00 ~ 21:00
- 地点：微信视频号“考鼎录”直播间
- [B 站直播回放](https://www.bilibili.com/video/BV1Wf4y1c7Ej?share_source=copy_web) <!-- .element: target="_blank" -->

		
## 提纲

1. 为什么不能忽视编译警告
1. 常见编译警告及其分类
1. 编译警告和函数属性
1. 消除编译警告只是开始

		
## 为什么不能忽视编译警告

- 大部分编译警告意味着代码存在缺陷或者问题
- 充斥大量编译警告的代码，通常意味着代码质量不高
- 破窗心理会拉低团队的平均水平

		
## 常见编译警告

Talk is cheap, show me the code.

		
## 将警告当做错误严肃对待

- 使用 `-Wall -Wextra -Werror`（以及一些额外的警告）选项，将常见或者重要的警告按错误处理。
- 错误太多时，使用 `-Wmax-errors=10` 及时停止编译。

注：本课程的编译警告分类以 GCC 为例，遵循 C99 规范。

		
## 编译警告的分类

- 预处理警告
- 未使用警告
- 未初始化警告
- 类型安全警告
- 格式化相关警告
- 词法警告
- 其他警告

		
## 预处理警告

- `-Wunused-macros`：未使用的宏
- `-Wundef`：使用未定义的宏（默认按 0 处理）
- `-Wtrigraphs`：无法识别的三联符

```c
??=include<stdio.h>
int main(void)
??<
printf("Hello World!\n");
return 0;
??>
```

		
## 未使用警告

	
容易理解的
- `-Wunused-label`：未使用的标签
- `-Wunused-variable`：未使用的变量
- `-Wunused-but-set-parameter`：未使用但被设置的参数
- `-Wunused-but-set-variable`：未使用但被设置的变量
- `-Wunused-local-typedefs`：未使用的局部类型定义

	
不容易理解的
- `-Wunused-parameter`：未使用的参数
- `-Wunused-value`：未使用的值
- `-Wunused-function`：未使用的函数
- `-Wunused-result`：未使用的结果

		
## 未初始化警告

- `-Wuninitialized`：未初始化的
- `-Wmaybe-uninitialized`：可能未初始化的
- `-Winit-self`：初始化自己

		
## 类型安全警告

	
强制转换相关
- `-Wcast-qual`：强制转换丢失限定符
- `-Wcast-align`：强制转换导致对齐单位增加
- `-Wcast-function-type`：强制转换函数指针到不兼容的（参数类型及返回值）函数指针
- `-Wdiscarded-qualifiers`：丢失限定符
- `-Wint-to-pointer-cast`：整数强制转换为指针
- `-Wpointer-to-int-cast`：指针强制转换为整数
- `-Wbad-function-cast`：强制转换函数的返回类型为不匹配的类型。

	
符号相关
- `-Wsign-compare`：对比有符号和无符号值时，由于隐含的符号转换会导致不正确的结果
- `-Wpointer-sign`：使用带符号的值赋值给指针变量或者传递给指针参数

	
隐含的类型转换相关
- `-Wconversion`：当隐式转换可能会改变值时
- `-Wsign-conversion`：隐含的转换会改变整数的符号
- `-Wfloat-conversion`：隐含的转换会丢失实数的精度
- `-Wenum-conversion`：一个枚举类型的值被隐含地转换为另一个枚举类型值
- `-Warith-conversion`：混合不同类型的运算时产生警告
- `-Wincompatible-pointer-types`：不兼容的指针类型

	
逻辑运算相关
- `-Wbool-compare`：当布尔表达式与不同于 `true`/`false` 的整数值相比较时。
- `-Wbool-operation`：在布尔类型表达式上存在可疑运算时。
- `-Wlogical-op`：奇怪的逻辑运算。
- `-Wlogical-not-parentheses`：在比较的左侧操作数上使用了逻辑“非”操作。

	
其他
- `-Wfloat-equal`：如果在相等比较中使用浮点值，则发出警告。
- `-Wpointer-arith`：警告任何依赖于函数类型或 void 之“大小”的运算。
- `-Wtype-limits`：由于数据类型的有限范围，当比较始终为真或始终为假时，则发出警告，但不对常量表达式发出警告。

		
## 格式化相关警告

	
- `-Wformat`：
   - 当提供给 `printf`、`scanf` 等格式化函数的参数和格式化字符串不匹配时。
   - 当格式化字符串的长度为零时；使用 `-Wno-format-zero-length` 关闭。
   - 当格式化字符串包含 NUL 字节时；使用 `-Wno-format-contains-nul` 关闭。
   - 当提供给 `printf` 或 `scanf` 函数的参数超过格式化字符串需要的参数个数时；使用 `-Wno-format-extra-args` 关闭。

	
- `-Wformat-overflow`：当提供给 `sprintf` 和 `vsprintf` 等格式化函数的缓冲区大小有溢出风险时。
- `-Wformat-security`：存在可能的格式化安全性问题，尤其是格式化字符串由外部传入时。
- `-Wformat-signedness`：格式化字符串要求的符号不匹配。
- `-Wformat-truncation`：对 `snprintf` 和 `vsnprintf` 的调用可能导致输出被截断。

		
## 词法警告

	
函数相关
- `-Wstrict-prototypes`：严格的函数原型。
- `-Wmissing-prototypes`：在定义全局函数之前，未见其原型声明。
- `-Wmissing-parameter-type`：未指定参数类型。
- `-Wredundant-decls`：冗余的声明。
- `-Wrestrict`：当一个对象被另一个带有约束限制的参数引用时，或者当这些对象之间的副本重叠时发出警告。
- `-Winline`：当被声明为 `inline` 的函数无法内嵌时，发生警告。

	
分支相关
- `-Wempty-body`：如果在 `if`、`else` 或 `do while` 语句中出现空体，则发出警告。
- `-Wduplicated-branches`：当 `if-else` 分支相同时。
- `-Wduplicated-cond`：当 `if-else-if` 中使用重复的条件时。
- `-Wdangling-else`：当出现可能混淆 `else` 分支所属的 `if` 语句时发出警告。
- `-Wimplicit-fallthrough`：`case` 语句落空。

	
结构相关
- `-Wmissing-field-initializers`：结构的初始化器当中缺少某些字段。
- `-Wpacked`：如果结构被赋予了压实（packed）属性，但压实属性对结构的布局或大小没有影响，则发出警告。
- `-Wpadded`：当为了对齐结构中的成员或者对齐整个结构而在结构中产生空白（padding）时，发出警告。
- `-Wpacked-not-aligned`：如果在压实的（packed）结构或联合中包含有一个未对齐的显式指定对齐的结构时，发出警告。

	
数组相关
- `-Wvla`：当使用可变长度的数组时发出警告。
- `-Wvla-larger-than`：当可变长度数组的尺寸大于指定值时产生警告。
- `-Wsizeof-array-argument`：当 sizeof 运算符应用于在函数定义中声明为数组的参数时。
- `-Warray-bounds`：数组下标始终越界时产生警告（和优化选项一并使用）。

		
## 其他

- `-Wabsolute-value `：当有更合适的标准函数可用时，对计算参数绝对值的不正确的标准函数调用发出警告。

		
## 函数属性及相关警告

- 函数属性用于辅助修饰一些函数的行为，以帮助编译器完成一些基本的代码分析，提示可能出现的缺陷。

	
`malloc` 属性
- 表明函数的行为类似 `malloc`
- `-Wfree-nonheap-object`：尝试释放未在堆上分配的对象或使用未从先前调用相应分配函数返回的指针时发出警告。

	
`nonnull` 属性

- 非空指针参数
- `-Wnonnull`：
- `-Wnonnull-compare`：

```c
void *memcpy(void *dst, const void *src, size_t n)
    __attribute__ ((nonnull (1, 2)));
```

	
`__warn_unused_result__` 属性
- 不使用返回值时产生警告
- `-Wunused-result`：
- `-Wno-unused-result`：

```c
int chdir(const char *path) __attribute__ ((__warn_unused_result__));
```

	
`format 属性`
- `format (archetype, string-index, first-to-check)`
- 指定函数具有类似 printf 的格式化字符串和可变参数
- `-Wformat`：检查格式化字符串和参数。

		
## BTW：其他常用函数属性

`const` 属性
- 函数对相同的参数返回相同的结果；用于优化。

```c
int square (int) __attribute__ ((const));
```
	
`deprecated` 属性
- 标记函数被废弃，将在未来移除；使用时产生 `-Wdeprecated`警告。

```c
int old_fn () __attribute__ ((deprecated));
```

	
`unavailable` 属性
- 标记函数不可用（已被移除）；使用时产生错误，无需等到链接时。

```c
int removed_fn () __attribute__ ((unavailable));
```

	
`noreturn` 属性
- 标记该函数不会返回，如标准 C 库函数 `exit` 和 `abort`。

```c
void fatal () __attribute__ ((noreturn));

void fatal (/* … */)
{
  /* … */ /* Print error message. */ /* … */
  exit (1);
}
```

	
`returns_nonnull` 属性
- 标记该函数的返回值不会为 NULL。

```c
extern void *
mymalloc (size_t len) __attribute__((returns_nonnull));
```

	
`visibility` 属性
- 标记外部/全局函数和变量的可见性；可取 `default`、`hidden`、`internal`、`protected` 四个值之一。

```c
void __attribute__ ((visibility ("internal"))) fn (void)
{
    /* Do something. */;
}

int i __attribute__ ((visibility ("hidden")));
```

		
## 不会被警告的缺陷

- `-Warray-bounds`：和优化选项一并使用
- `-Wmaybe-uninitialized`：仅在优化编译时可用
- `-Wuninitialized`：并不是万能的

		
## Q & A

