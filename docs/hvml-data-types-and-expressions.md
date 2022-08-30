## HVML 的数据类型和表达式

- HVML 规范文档：<https://github.com/HVML/html-docs>
- HVML 解释器：<https://github.com/HVML/PurC>

		
## 基本数据类型

1. 兼容 JSON
1. 基本数据项类型
   - 空：`null`
   - 布尔（boolean）：`true`, `false`
   - 数值（number）：`3.1415926`, `-2022`
   - 字符串（string）：`"好可爱的猫咪~"`
1. 基本容器类型
   - 数组
   - 对象

	

### 示例

```js
// 数组
[ null, true, -2022 ]

// 对象
{
    "name": "HVML",
    "birthland": "中国",
    "nickname": "呼噜猫",
    "age": 1
}
```

		
## 扩展数据类型

1. 未定义（undefined）：该数据仅在运行时使用，用来表示数据尚未就绪，或在更新容器数据时移除一个成员。
1. 异常（exception）：该数据仅在内部使用，用来表示异常。
1. 有符号长整数（longint）：`0xABCDEFL`, `-987L`；实现为 64 位有符号整数。
1. 无符号长整数（ulongint）：`0xABCDEFUL`, `-1UL`；实现为 64 位无符号整数。
1. 长浮点数（longdouble）：对应 C 语言 long double 类型。

	
1. 字节序列（bsequence）：`bx001122334455`, `bb0011.1100`；可理解为 C 语言的字节数组。
1. 集合（set）：特殊的数组，其中的成员可根据其值或者对象数组上的唯一性键值确保唯一性；通常使用数组初始化，仅在运行时存在。
1. 元组（tuple）：可使用索引引用的多个数据项，一旦创建，其容量不可变，其成员不可移除，仅可置空。

		
## 动态数据类型

- 动态值：动态值本质上由获取器（getter）和设置器（setter）方法构成；读取时，由获取器返回对应的值，设置时，由设置器完成对应的工作。
- 原生实体：由底层实现的原生实体，通常用于代表一些可执行复杂操作的抽象数据，如读写流、通道等。
- 动态值和原生实体构成了 HVML 的动态对象，通常通过变量名和表达式访问：

```js
$SYS.time

$SYS.time(! 0 )
```

		
## 通过变量引用数据

1. 必要的预定义变量
   - `$SYS`：获取系统信息，比如时间、时区、区域、当前路径、系统环境变量等。
   - `$STR`：字符串操作，比如取子字符串。
   - `$L`：逻辑运算，对比大小、匹配字符串等。
   - `$EJSON`：获取数据相关信息，完成数据类型转换，基本的四则运算、位运算等。
   - `$DATETIME`：有关日期时间的操作。

	
1. 可选的预定义变量
   - `$MATH`：数学计算。
   - `$FS`：文件系统操作。
   - `$FILE`：文件操作。

	
1. 自定义变量
   - `init` 标签，将表达式的求值结果和一个名称绑定在一起。

```hvml
$SYS.locale

$SYS.locale('time')

<init as 'hvml'>
    {
        "name": "HVML",
        "birthland": "中国",
        "nickname": "呼噜猫",
        "age": 1,
        "software": ['PurC', 'xGUI Pro', 'PurC Fetcher', 'DOM Ruler' ]
    }
</init>

$hvml.name

$hvml.nickname

$hvml.software[0]

$hvml.software[-1]
```

		
## 表达式的使用

计算两个正整数的最大公约数

```hvml
# RESULT: [ 3L, 11L, 1L ]

<!-- Greatest Common Divisor -->

<!DOCTYPE hvml>
<hvml target="void">

    <init as 'result' with [] temp />

    <define as "calcGreatestCommonDivisor">
        <test with $L.or($L.le($x, 0), $L.le($y, 0)) >
            <return with undefined />
        </test>

        <!-- We use the compound EJSON expression to have the same result
             like `(x > y) ? x : y` in C language -->
        <init as "big" with {{ $L.gt($x, $y) && $x || $y }} temp />
        <init as "small" with {{ $L.lt($x, $y) && $x || $y }} temp />

        <test with $L.eq($EJSON.arith('%', $big, $small), 0) >
            <return with $small >
                $STREAM.stdout.writelines("returns $small for $small and $big")

            </return>
        </test>

        <!-- Note that `$0<` refers to the context variable `<`
            in the current stack frame -->
        <iterate on $EJSON.arith('/', $small, 2) onlyif $L.gt($0<, 0)
                with $EJSON.arith('-', $0<, 1) nosetotail >

            <test with $L.eval('a == 0 && b == 0',
                    { a: $EJSON.arith('%', $big, $?),
                      b: $EJSON.arith('%', $small, $?) }) >
                <return with $?>
                    $STREAM.stdout.writelines("returns $? for $small and $big")

                </return>
            </test>

        </iterate>

        <return with 1L >
            $STREAM.stdout.writelines("returns 1 for $small and $big")

        </return>

    </define>

    <call on $calcGreatestCommonDivisor with { x: 3L, y: 6L } >
        <update on $result to 'append' with $? />
    </call>

    <call on $calcGreatestCommonDivisor with { x: 33L, y: 11L } >
        <update on $result to 'append' with $? />
    </call>

    <call on $calcGreatestCommonDivisor with { x: 37L, y: 11L } >
        <update on $result to 'append' with $? />
    </call>

    $result
</hvml>
```

		
## 字符串置换表达式

1. 双引号和单引号的区别：求值和转义

	
```js
<init as 'hvml'>
    {
        "name": "HVML",
        "birthland": "中国",
        "nick name": '呼噜"猫"',
        "age": 1,
        "software": ['PurC', 'xGUI Pro', 'PurC Fetcher', 'DOM Ruler' ]
    }
</init>

"$hvml.name 来自 $hvml.birthland，年龄 $hvml.age 岁，小名：$hvml['nick name']。"
```

		
## 复合表达式

```js
// 调用 $SYS.cwd 将当前工作路径切换到 `/etc` 目录下，然后调用 $FS.list
// 获得所有目录项对象数组。
{{ $SYS.cwd(! '/etc'); $FS.list }}

// 尝试改变工作路径到 `/root` 目录下，如果成功则调用 $FS.list 获得该目录下
// 所有目录项对象数组，否则向标准输出（$STREAM.stdout）打印提示信息，
// 并改变工作路径到 `/` 下，若成功，则获得该目录下所有目录项对象数组，
// 否则将 `false` 作为该 CJSONEE 的最终求值结果。
{{
     $SYS.cwd(! '/root') &&
        $FS.list ||
        $STREAM.stdout.writelines(
                'Cannot change directory to "/root"'); $SYS.cwd(! '/' ) &&
                    $FS.list || false
}}

// 尝试改变工作路径到 `/root` 目录下，如果成功则调用 $FS.list_prt 获得该目录下
// 所有目录项清单（字符串），否则返回提示信息。最终将目录项清单或者错误信息
// 输出到标准输出。
{{
    $STREAM.stdout.writelines({{
                $SYS.cwd(! '/root') && $FS.list_prt ||
                    'Cannot change directory to "/root"'
            }})
}}
```

		
## 更加实用的 eJSON 语法

```js
{
    name: 'HVML',
    birthland: '中国',
    'nick name': '呼噜猫',
    age: 1,
    software: [ 'PurC', 'xGUI Pro', 'PurC Fetcher', 'DOM Ruler', ],
}
```

		
## 使用汉字作为变量名

- 正在实现中

```js
<init as '呼噜猫'>
    {
        名称: 'HVML',
        出生地: '中国',
        昵称: '呼噜猫',
        年龄: 1,
        软件: [ 'PurC', 'xGUI Pro', 'PurC Fetcher', 'DOM Ruler', ],
    }
</init>

"$呼噜猫.名称 来自 $呼噜猫.出生地，年龄 $呼噜猫.年龄 岁，小名 $呼噜猫['昵称']。"
```
	
- 若不希望出现空格时使用 {$...} 这种表示法

```js

"{$呼噜猫.名称}来自$呼噜猫.出生地，年龄{$呼噜猫.年龄}岁，小名$呼噜猫['昵称']。"
```

		
## 表达式的使用

1. 表达式主要用于设定元素的属性值以及内容。
1. HVML 元素中，内容中的表达式可用来设定该程序的执行结果。
1. 动作元素的内容，由单个表达式的求值结果确定，不能包含的多个表达式。
1. 任何一个无法通过求值产生合法结果数据的表达式都是错误的。

	
错误用法

```hvml
<init as "呼噜 猫" >
    $STREAM.stdout.writelines("Hello, world!")
    $STREAM.stdout.writelines("Hello, world!")
</init>
```

正确用法

```hvml
<init as 'hvml' >
    {{
        $STREAM.stdout.writelines("Hello, world!");
        $STREAM.stdout.writelines("Hello, world!")
    }}
</init>
```

		
## 有关表达式的持续演进

1. 预定义变量使用中文表示预定义变量和属性或方法的名称。
1. 直接支持算术运算表达式和逻辑运算表达式。

