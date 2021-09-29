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
- 未使用的 XXX 警告
- 未初始化的 XXX 警告
- 类型安全警告
- 格式化安全警告
- 词法警告
- 其他警告

		
## 预处理警告

- `-Wundef`：使用未定义的宏（默认按 0 处理）
- `-Wtrigraphs`：三联符

```c
??=include<stdio.h>
int main(void)
??<
printf("Hello World!\n");
return 0;
??>
```

		
## 未使用的 XXX 警告

容易理解的：
- `-Wunused-macros`：未使用的宏
- `-Wunused-label`：未使用的标签
- `-Wunused-variable`：未使用的变量
- `-Wunused-parameter`：未使用的参数
- `-Wunused-but-set-parameter`：未使用但被设置的参数
- `-Wunused-but-set-variable`：未使用但被设置的变量
- `-Wunused-local-typedefs`：未使用的局部类型定义

	
不容易理解的：
- `-Wunused-value`：未使用的值
- `-Wunused-function`：未使用的函数
- `-Wunused-result`：未使用的结果

		
## 未初始化的 XXX 警告

- `-Wuninitialized`：未初始化的
- `-Wmaybe-uninitialized`：可能未初始化的
- `-Winit-self`：初始化自己

		
## 不会被警告的缺陷

- `-Wmaybe-uninitialized`：仅在优化编译时可用
- `-Wuninitialized`：不能用来判断内容是否初始化

		
## Q & A

