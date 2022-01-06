## 第九讲：状态机

- 时间：2022 年 1 月 6 日 17:30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 状态机所要解决的问题
1. 状态机相关理论
1. 正确理解状态机
1. 状态机的软件实现
1. 已知状态实现有限状态机
1. 已知迁移规则实现自定义状态
1. 实例1：HTML 解析器中的分词器
1. 实例2：动画状态机

		
## 状态机所要解决的问题

1. 计算机最擅长处理重复性的工作。
1. 现实世界中大量事物的工作流程取决于其他事物的状态。
1. 状态机就是针对多种状态之间的迁移而建立的一个简单数学模型。
1. 状态机可用来指导硬件模块或者软件模块的设计。

	
### 状态机的概念

1. 一个硬件或者一个软件模块；
1. 我们给状态机一个初始状态；
1. 然后给状态机一个事件；
1. 状态机根据事件和当前状态完成一个动作或者转换到另外一个状态；
1. 终止执行或者等待下一个事件。

如此，状态机就成了一个可重复工作的机器。

		
## 状态机相关理论

1. 术语
   - 状态（State）；可分为当前状态（现态）和迁移后的新状态（次态）。
   - 事件（Event）；又称为条件。
   - 动作（Action）。
   - 迁移、转换或变换（Transition）。
1. 分类
   - Moore 状态机：次态无关事件。
   - Mealy 状态机：次态和现态及事件均有关。

	
### 方法

1. 状态迁移图
1. 状态迁移表

		
## 正确理解状态机

- 问题：是不是所有含有条件分支代码的程序或者函数都可以看成是一个状态机？

	
### 非状态机：根据事件执行相应动作

- 简单的事件驱动模型，并不是状态机。
- 比如，如下代码判断输入字符的类型：

```c
#include <stdio.h>
#include <ctype.h>

int main(void)
{
    int c;

    while (1) {
        unsigned char uc;

        c = getchar();
        uc = (unsigned char)c;

        if (isdigit(c)) {
            int v = c - 48;
            printf("Got a digit character: %d\n", v);
        }
        else if (isupper(c)) {
            printf("Got a upper case letter: %c\n", c);
        }
        else if (islower(c)) {
            printf("Got a lower case letter: %c\n", c);
        }
        else if (c == EOF) {
            printf("Got EOF\n");
            break;
        }
        else if (uc >= 0x20 && uc < 0x80) {
            printf("Got a non-digit-and-alpha printable character: %c\n", c);
        }
        else if (uc < 0x20) {
            printf("Got a control character: 0x%x\n", c);
        }
        else {
            printf("Got a non-ASCII character: 0x%x\n", c);
        }
    }

    return 0;
}
```

	
### 状态机：必须存在状态迁移

比如我们要解析 JSON 数据：

```json
{
    "object": {},
    "array": [],
    "string": "This is a string",
    "number": 4567,
    "true": true,
    "false": false,
    "null": null
}
```

[JSON 规范](https://www.json.org/json-zh.html)

		
## 状态机的软件实现

	
1. 已知状态实现固定的有限状态机

	
1. 已知迁移规则实现自定义状态机

		
1. 实例1：HTML 解析器中的分词器

		
1. 实例2：动画状态机

		
## 下一讲预告

- 主题：为性能设计
- 地点：微信视频号“考鼎录”直播间
- 时间：待定

		
## Q & A

