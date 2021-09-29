## 第三讲：清除所有编译警告

- 时间：9 月 30 日 20:00 ~ 21:00
- 地点：微信视频号“考鼎录”直播间
- [立即预约](#/3)

		
## 提纲

- 为什么不能忽视编译警告
- 常见编译警告及其分类
- 并不是所有编译警告都意味着缺陷
- 难以通过编译警告发现的缺陷

		
## 为什么不能忽视编译警告

- 大部分编译警告意味着代码存在缺陷或者问题
- 充斥大量编译警告的代码，通常意味着代码质量不高
- 破窗心理会拉低团队的平均水平

		
## 常见编译警告

Talk is cheap, show me the code.

		
## 将警告当做错误严肃对待

- 使用 `-Wall -Wextra -Werror` 选项，将所有警告按错误处理。
- 错误太多时，使用 `-Wmax-errors=10` 及时停止编译。

		
## 编译警告的分类

- 预处理警告
- 未使用警告
- 未初始化警告
- 类型安全警告
- 格式化安全警告
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

容易理解的：
- `-Wunused-label`：未使用的标签
- `-Wunused-variable`：未使用的变量
- `-Wunused-but-set-parameter`：未使用但被设置的参数
- `-Wunused-but-set-variable`：未使用但被设置的变量
- `-Wunused-local-typedefs`：未使用的局部类型定义

	
不容易理解的：
- `-Wunused-parameter`：未使用的参数
- `-Wunused-value`：未使用的值
- `-Wunused-function`：未使用的函数
- `-Wunused-result`：未使用的结果

		
## 未初始化警告

- `-Wuninitialized`：未初始化的
- `-Wmaybe-uninitialized`：可能未初始化的
- `-Winit-self`：初始化自己

		
## 类型安全警告

强制转换相关：
- `-Wcast-qual`：强制转换丢失限定符
- `-Wcast-align`：强制转换导致对齐单位增加
- `-Wcast-function-type`：强制转换函数指针到不兼容的（参数类型及返回值）函数指针
- `-Wbad-function-cast`：强制转换函数的返回类型为不匹配的类型。
- `-Wint-to-pointer-cast`：整数强制转换为指针。
- `-Wpointer-to-int-cast`：指针强制转换为整数。

	
符号相关：
- `-Wsign-compare`：对比有符号和无符号值时，由于隐含的符号转换会导致不正确的结果。
- `-Wpointer-sign`：使用带符号的值赋值给指针变量或者传递给指针参数。

	
隐含的类型转换：
- `-Wsign-conversion`：隐含的转换会改变整数的符号。
- `-Wfloat-conversion`：隐含的转换会丢失实数的精度。
- `-Wenum-conversion`：一个枚举类型的值被隐含地转换为另一个枚举类型值。
- `-Warith-conversion`：

		
## 格式化安全警告

- `-Wformat`

		
## 词法警告

- `-Wstrict-prototypes`
- `-Wmissing-parameter-type`
- `-Wempty-body`
- `-Wmissing-field-initializers`
- `-Woverride-init`
- `-Wvla`
- `-Wsizeof-array-argument`

		
## 其他警告

		
## 函数属性及相关警告

`nonnull`属性：非空指针参数

- `-Wnonnull`：
- `-Wnonnull-compare`：

```c
void *memcpy(void *dst, const void *src, size_t n)
    __attribute__ ((nonnull (1, 2)));
```

	
`__warn_unused_result__` 属性：不使用返回值时产生警告

- `-Wunused-result`：
- `-Wno-unused-result`：

```c
int chdir(const char *path) __attribute__ ((__warn_unused_result__));
```

		
## BTW：其他常用函数属性

`const` 属性：函数对相同的参数返回相同的结果；用于优化。

```c
int square (int) __attribute__ ((const));
```

	
`deprecated` 属性：标记函数被废弃，将在未来移除；使用时产生 `-Wdeprecated`警告。

```c
int old_fn () __attribute__ ((deprecated));
```

	
`unavailable` 属性：标记函数不可用（已被移除）；使用时产生错误，无需等到链接时。

```c
int removed_fn () __attribute__ ((unavailable));
```

	
`noreturn` 属性：标记该函数不会返回，如标准 C 库函数 `exit` 和 `abort`。

```c
void fatal () __attribute__ ((noreturn));

void fatal (/* … */)
{
  /* … */ /* Print error message. */ /* … */
  exit (1);
}
```

	
`returns_nonnull` 属性：标记该函数的返回值不会为 NULL。

```c
extern void *
mymalloc (size_t len) __attribute__((returns_nonnull));
```

	
`visibility` 属性：标记外部/全局函数和变量的可见性；可取 `default`、`hidden`、`internal`、`protected` 四个值之一。

```c
void __attribute__ ((visibility ("internal"))) fn (void)
{
    /* Do something. */;
}

int i __attribute__ ((visibility ("hidden")));
```

		
## 不会被警告的缺陷

- `-Wmaybe-uninitialized`：仅在优化编译时可用
- `-Wuninitialized`：并不是万能的

		
## Q & A

