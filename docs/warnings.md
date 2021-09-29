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

- 大部分编译警告意味者代码存在缺陷或者问题
- 充斥大量编译警告的代码，通常意味着代码质量不高
- 破窗心理会拉低团队的平均水平

		
## 常见编译警告

Talk is cheap, show me the code.

		
## 编译警告的分类

- 预处理警告
- 未使用的 X 警告
- 类型安全警告
- 格式化安全警告
- 词法警告
- 其他警告

		
## 未使用的 X 警告

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

		
## Q & A

