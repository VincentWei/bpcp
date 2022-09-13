## HVML 概览

- HVML 规范文档：<https://github.com/HVML/html-docs>
- HVML 解释器：<https://github.com/HVML/PurC>

		
## 挑战

- 开发者是基础软件的第一用户
   1. 一个基础软件，不论是操作系统还是数据库，必须首先满足开发者的需求，服务甚至取悦于开发者。
   1. 基础软件及其生态的价值，会被开发者成百倍地放大。
   1. 操作系统之间的竞争，本质上是对开发者的争夺战。
   1. 我们看到微软、苹果、谷歌，都在不遗余力开发各种编程语言、应用框架和开发工具。
- 但中国基础软件界在编程语言、应用框架和开发工具方面鲜有建树。

	
### 我自己的切身体会

- 开发效率的提升，对开发者意义重大
   1. MiniGUI、Gtk、Qt 采用的是 C/C++ 接口，开发者的时间浪费在对大量界面细节的调整上。
   1. Web 前端技术广泛使用 HTML/CSS 等描述式语言，方便界面样式、布局以及动画的开发，大大提升了开发效率。
- 未来的应用，要同时支持云端、桌面、智能手机、平板、手表以及未来的 AR/VR 等平台，因此，跨平台的需求越来越强烈。
- 苹果、谷歌、微软，均在为 Universal App 做技术准备。

	
### 纲举目张，新式编程语言是操作系统制胜未来的法宝

- 描述式语言，易读且容易理解，甚至可支持开发者使用母语编程。
- 具有更高抽象层次的编程语言；开发者可以使用更少的代码实现更多的工作，且无需过多关心技术细节。
- 提供跨平台可移植接口的编程语言；通过全新的接口设计来屏蔽底层操作系统或者平台的差异。
- 支持现代编程技术，如动态、协程、并发、闭包等。
- 良好的可扩展性和伸缩性，较多的应用场合。
- 功能和性能的良好平衡。

		
## 初衷和收获

- 起先，我们的目标是让熟悉 C/C++ 或其他编程语言的开发人员可以通过 HVML 使用 Web 前端技术轻松开发 GUI 应用程序，而不是使用 Electron 这样的庞然大物。
- 现在，我们不光实现了这一目标，而且还将 HVML 实现为一种通用的编程语言。

		
## 定义

- HVML 是 `Hybrid Virtual Markup Language` 的缩写。
- HVML 读作 /huːmɪl/，音似普通话读“虎猫”或“呼喵”。
- HVML 的中文名叫“呼噜猫”。
- HVML 是一种可编程标记语言。
- HVML 不同于常见的高级编程语言（C/C++/Java/Rust），也不同于常见的脚本语言（JavaScript/PHP/Python 等）。

		
## 技术特征(1/8)：描述式编程语言

- HVML 使用类似 XML 的标记语言来定义程序结构。
- HVML 定义了为数不多的十多个动作标签，可用来定义变量，操作数据或者控制程序的执行路径。
- HVML 使用介词属性和副词属性来定义动作依赖的源数据、目标数据以及执行条件，从而获得更加贴近自然语言的程序表达和书写效果。
- HVML 允许我们混合使用外部标签，从而可以非常方便的生成目标文档内容。

	
### 将数据绑定到变量

```hvml
<init as 'emptyArray' with [] />

<init as 'users'>
    [
        { "id": "1", "avatar": "/img/avatars/1.png", "name": "Tom",
            "region": "en_US", "age": 2 },
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "Jerry",
            "region": "zh_CN", "age": 3 }
    ]
</init>
```

	
### 定义集合

```hvml
<init as 'users' uniquely against 'id'>
    [
        { "id": "1", "avatar": "/img/avatars/1.png", "name": "Tom", "region": "en_US" },
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "Jerry", "region": "zh_CN" }
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "David", "region": "zh_CN" }
    ]
</init>
```

	
### 从外部获取数据定义变量

```hvml
<init as 'users' from 'https://foo.bar.com/users/$SYS.locale' uniquely against 'id' async >
    [
        { "id": "1", "avatar": "/img/avatars/1.png", "name": "Tom", "region": "en_US" },
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "Jerry", "region": "zh_CN" }
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "David", "region": "zh_CN" }
    ]
</init>
```

	
### 嵌套使用外部元素

```hvml
<ul>
    <init as 'users' from 'https://foo.bar.com/users' uniquely against 'id' >
        <iterate on $users >
            <li>$?.name</li>
        </iterate>
    </init>
</ul>
```

		
## 技术特征(2/8)：灵活的表达式

- HVML 表达式通常用于定义元素的属性值及内容。
- HVML 在广泛使用的 JSON 表述方法之上，使其具有了动态处理能力以及参数化表述数据的能力。
- HVML 扩展了 JSON 表述方法，使之支持更多数据类型，并通过使用动态值和原生实体这两类动态数据，定义了从底层系统获得数据或者实现某种功能的方法。

比如下面的 HVML 代码片段，通过表达式 `$STR.substr($SYS.locale, 0, 2)` 取系统区域字符串（如 `zh_CN`）的前两个字符作为结果，设置了 `lang` 这个属性的属性值（`zh`）：

```hvml
<hvml target="html" lang="$STR.substr($SYS.locale, 0, 2)">
    ...
</hvml>
```

	
### 动态对象

1. 必要的预定义动态对象
   - `$SYS`：获取系统信息，比如时间、时区、区域、当前路径、系统环境变量等。
   - `$STR`：字符串操作，比如取子字符串。
   - `$L`：逻辑运算，对比大小、匹配字符串等。
   - `$EJSON`：获取数据相关信息，完成数据类型转换，基本的四则运算、位运算等。
   - `$DATETIME`：有关日期时间的操作。
   - `$URL`：有关 URL 处理的操作。
1. 可选的预定义动态对象
   - `$MATH`：数学计算。
   - `$FS`：文件系统操作。
   - `$FILE`：文件操作。
1. 自定义变量
   - 通过 `init` 标签从动态库中动态装载。

	
### 复合表达式

- 由多个单一表达式组成，具有简单的逻辑控制能力

```hvml
{{
    // 尝试改变工作路径到 `/root` 目录下
    $SYS.cwd(! '/root') &&
        // 如果成功则调用 $FS.list 获得该目录下的所有目录项对象数组
        $FS.list ||
            // 否则向标准输出（$STREAM.stdout）打印提示信息
            $STREAM.stdout.writelines('Cannot change directory to "/root"');
            // 并改变工作路径到 `/` 下
            $SYS.cwd(! '/' ) &&
                // 若成功，则获得该目录下所有目录项对象数组
                $FS.list ||
                    // 否则将 `false` 作为该 CJSONEE 的最终求值结果
                    false
}}
```

	
### 字符串置换表达式

	
```hvml
<init as 'hvml'>
    {
        "name": "HVML",
        "birthland": "中国",
        "nick name": '呼噜"猫"',
        "age": 1,
        "software": ['PurC', 'xGUI Pro', 'PurC Fetcher', 'DOM Ruler' ]
    }
</init>

<init as 'sentence'
    with "$hvml.name 来自 $hvml.birthland，年龄 $hvml.age 岁，小名：$hvml['nick name']。" />
```
		
## 技术特征(3/8)：数据驱动

- 在 HVML 中，我们倡导围绕要处理的数据组织程序结构，比如选择数据，在数据上迭代，执行规约或者排序操作，观察数据的变化等等。
- 在 HVML 中，我们还可以通过更改数据来操控某个功能的实现，比如增加一个定时器，我们不需要调用某个编程接口，而是在一个集合中新增一个数据项。

```hvml
<!-- 新增标识符为 `foo` 的定时器，间隔 3000 ms，激活状态 -->
<update on "$TIMERS" to "append">
    { "id" : "foo", "interval" : 3000, "active" : "yes" }
</update>

...

<!-- 使标识符为 `foo` 的定时器失效 -->
<choose on "$TIMERS" by "FILTER: AS 'foo'">
    <update on $? at ".active" with "no" />
</choose>
```

		
## 技术特征(4/8)：模板及文档操作

- 通过使用参数化的扩展 JSON 表达式来定义文档片段模板或者数据模板，避免在程序代码中调用特定接口来修改目标文档，HVML 程序只需要关注数据本身的产生和处理即可。
- HVML 对文档和数据的操作提供了一致的动作元素（`update`）。
- 方便实现界面和数据的解耦。

	
### 文档模板

```hvml
<init as "users">
    [
        { "id": "1", "avatar": "/img/avatars/1.png", "name": "Tom",
            "region": "en_US", "age": 2 },
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "Jerry",
            "region": "zh_CN", "age": 3 }
    ]
</init>

<archetype name="user_item">
    <li class="user-item" id="user-$?.id"
            data-value="$?.id" data-region="$?.region">
        <img class="avatar" src="$?.avatar" />
        <span>$?.name</span>
    </li>
</archetype>

<ul class="user-list">
    <iterate on "$users" by "RANGE: FROM 0">
        <update on $@ to "append" with $user_item />
    </iterate>
</ul>
```
	
### 数据模板

		
## 技术特征(5/8)：栈式虚拟机

	
### 闭包

	
### 操作组

	
### 调用和就地执行

	
### 回退及异常处理

		
## 技术特征(6/8)：事件驱动

	
### 程序的两个执行阶段

		
## 技术特征(7/8)：异步及并发编程

	
### 协程和异步

	
### 表达式的重新求值

	
### 行者及并发调用

	
### 请求及响应

		
## 技术特征(8/8)：动态性

变量名、表达式、操作组、程序，皆可动态生成。
变量可被移除。

		
## 典型 HVML 程序

	
## 计算两个正整数的最大公约数

```hvml
<!-- 最大公约数 -->

<!DOCTYPE hvml>
<hvml target="void">

    <init as 'result' with [] temp />

    <define as "calcGreatestCommonDivisor">
        <test with $L.or($L.le($x, 0), $L.le($y, 0)) >
            <return with undefined />
        </test>

        <!-- 我们使用复合表达式来获得与 C 语言 `(x > y) ? x : y` 语句相同的结果 -->
        <init as "big" with {{ $L.gt($x, $y) && $x || $y }} temp />
        <init as "small" with {{ $L.lt($x, $y) && $x || $y }} temp />

        <!-- 如果 `$big` 和 `$small` 互质，则直接返回 `$small` 作为结果 -->
        <test with $L.eq($EJSON.arith('%', $big, $small), 0) >
            <return with $small >
                $STREAM.stdout.writelines("returns $small for $small and $big")

            </return>
        </test>

        <!-- 这里的 `$0<` 指的是当前栈帧中的上下文变量 `<`（表述迭代的输入数据）。
            其中，`on` 介词属性定义了迭代的输入数据，`onlyif` 介词属性定义了迭代条件，
            `with` 介词属性定义了执行一次迭代后的求值表达式，而 `nosetotail`
            这个副词属性表示将将 `with` 属性值作为下个迭代的输入数据。

            这个 `iterate` 元素的含义为：
                初始的输入数据为 `$small/2`；当输入数据大于 0 时执行当前迭代；
                每执行一次迭代将输入数据减 1，并使用该结果作为下一个迭代的输入数据。
        -->
        <iterate on $EJSON.arith('/', $small, 2) onlyif $L.gt($0<, 0)
                with $EJSON.arith('-', $0<, 1) nosetotail >

            <!-- 这里使用了 `$L` 变量的 `eval` 方法，用于测试一个参数化逻辑表达式（`a == 9 && b == 0`）
                 的值是真（true）还是假（false），而逻辑表达式中参数 a 和 b 的值，由传入 `eval` 方法的对象给定。-->
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

    <!-- 调用 `$calcGreatestCommonDivisor` 操作组，参数 x 是 3L, y 是 6L -->
    <call on $calcGreatestCommonDivisor with { x: 3L, y: 6L } >
        <!-- 将上下文变量 `$?` 的值追加（append）到 `$result` 代表的数组中；
             `$?` 就是上面 `call` 元素的执行结果，在这里就是 3L -->
        <update on $result to 'append' with $? />
    </call>

    <call on $calcGreatestCommonDivisor with { x: 33L, y: 11L } >
        <update on $result to 'append' with $? />
    </call>

    <call on $calcGreatestCommonDivisor with { x: 37L, y: 11L } >
        <update on $result to 'append' with $? />
    </call>

    <!-- `$result` 代表的数据将作为 hvml 元素的内容，最终成为整个程序的执行结果。
        该结果应该是：`[3L, 11L, 1L]`
    -->
    $result
</hvml>
```

	
## 生产者消费者

```hvml
# RESULT: 'HVML'

<!-- The expected output of this HVML program will be like:

2022-08-24T12:27:00+08:00: the data received: H
2022-08-24T12:27:01+08:00: the data received: V
2022-08-24T12:27:02+08:00: the data received: M
2022-08-24T12:27:03+08:00: the data received: L
2022-08-24T12:27:03+08:00: The result got from the reader: HVML

-->

<hvml target="void">
    <body>

        <!-- open a channel named `myChannel` -->
        <init as chan with $RUNNER.chan(! 'myChannel' ) />

        <!-- start the writer coroutine asynchronously -->
        <load from "#writer" asynchronously />

        <!-- start the reader coroutine and wait for the result -->
        <load from "#reader">
            $STREAM.stdout.writelines("The result got from the reader: `$?`")

            <exit with $? />
        </load>

    </body>

    <body id="writer">
        <init as chan with $RUNNER.chan('myChannel') />

        <iterate on [ 'H', 'V', 'M', 'L' ]>
            $chan.send($?)

            <sleep for '1s' />

        </iterate>

        <!-- close the channel -->
        <inherit>
            $RUNNER.chan(! 'myChannel', 0)
        </inherit>

    </body>

    <body id="reader">
        <choose on $RUNNER.chan('myChannel')>

            <init as result with '' />

            <!-- the channel has been closed if $chan.recv() returns false -->
            <iterate with $?.recv() silently>
                $STREAM.stdout.writelines("$DATETIME.time_prt: the data received: $?");

                <init as result at '_grandparent' with "$result$?" />
            </iterate>

            <exit with $result />
        </choose>

    </body>

</hvml>
```

	
### 素数筛

```hvml
# RESULT: [ 2L, 3L, 5L, 7L, 11L, 13L, 17L, 19L, 23L, 29L ]

<!DOCTYPE hvml>
<hvml target="void">

    <body id="main">
        <!-- update on $RUNNER.myObj to "merge" with { 'primes': [] } / -->

        <inherit>
            $RUNNER.user(! 'primes', [] )
        </inherit>

        <init as chIn with $RUNNER.chan(! "natural" ) temp />

        <load from "#initialNumbers" with { chanName: 'natural', diff: 1L } async />

        <!-- iterate 10 times -->
        <iterate on 0L onlyif $L.lt($0<, 10L) with $EJSON.arith('+', $0<, 1) nosetotail >

            <choose on $chIn.recv() >

                <update on $RUNNER.myObj at 'primes' to "append" with $? />

                <!-- open a channel to filter the new prime -->
                <init as chOut with $RUNNER.chan(! "filter$?" ) temp />

                <load from "#sieve" with { 'in': $chIn, "out": $chOut, 'prime': $? } async />

                <!-- make the output channel to be the input channel -->
                <init as chIn at "#main" with $chOut temp />
            </choose>

        </iterate>

        <!-- close the channel named `natural` -->
        <init as chIn with $RUNNER.chan(! "natural", 0 ) temp />

        <exit with $RUNNER.myObj.primes />
    </body>

    <!-- request: {chanName: <string: `the channel name`>, diff: <longint: `the difference`> } -->
    <body id="initialNumbers">

        <!-- open the channel -->
        <init as chan with $RUNNER.chan( $REQ.chanName ) />

        <!-- this iteration will stop only if the the channle was closed -->
        <iterate on 2L onlyif $chan.send($0<) with $EJSON.arith('+', $0<, $REQ.diff) nosetotail silently />
    </body>

    <body id="sieve">

        <!-- this iteration will stop only if the the input channle was closed -->
        <iterate on $REQ.in.recv() silently >
            <test with $L.ne($EJSON.arith('%', $?, $REQ.prime), 0L) >
                $REQ.out.send($2?)
            </test>
        </iterate>

        <!-- close the output channel -->
        <inherit>
            $RUNNER.chan(! "filter$REQ.prime", 0 )
        </inheirt>
    </body>
</hvml>
```

		
## 应用框架

- 程序逻辑和界面渲染的解耦
- 四种架构

		
## 应用场景

- 跨平台移固融合应用开发
- 桌面应用（Electron）
- 用于 C/C++/Rust 的内嵌胶水语言（Lua）
- 独立脚本语言（Python）
- 高并发服务器应用（PHP）

		
## 现状及路线图

- 2022.07.31 在 GitHub 上开源，含六个软件仓库/软件包
   1. 解释器 PurC
   1. 数据获取器 PurC Fetcher
   1. 字符渲染器 PurC Midnight Commander
   1. DOM 布局器：DOM Ruler
   1. 图形渲染器 xGUI Pro
   1. 定制 WebKit 引擎（软件包）

	
- 总代码行数：100M+
- 解释器仓库开源一个月获 1000+ 星星
- 支持 Linux 及 macOS 操作系统

	
- 2022.09 发布 PurC 0.8.2
  - 变量名可使用汉字（Unihan 表意字符）
  - 支持重新求值及通道
  - 优化的执行栈和求值栈输出

	
- 2022.10 发布 PurC 0.9.x
  - 支持元组
  - 内建字符渲染器

	
- 2022.12 发布 PurC 1.0.0
  - 所有 HVML 规范 1.0 定义的特性
  - 所有 HVML 预定义变量规范 1.0 定义的特性
  - 支持 Windows

	
- 2023.12 发布 PurC 2.0
  - 中文编程语言
  - 支持 iOS、Android

		
## 关键应用：HybridOS

- 定位：嵌入式操作系统
- 对标：Yocto 项目
- 基于 Linux 内核
- 图形栈基于 MiniGUI
- hiWebKit 已就绪
- 数据总线 hiBus 已就绪
- 支持多款国产 ARM SoC
- 目标：打败 Qt

	
- 2022.12 发布 R22.12（预览版）
  - 基于 MiniGUI 及 hiWebKit 的渲染器 xGUI
  - 使用 PurC 的参数化 JSON 处理能力重构 hiBus
  - 引入 HVML 应用框架

	
- 2023.12 发布 R23.12（正式版）
  - 基于 MiniGUI 及 hiWebKit 的渲染器 xGUI
  - 使用 PurC 的参数化 JSON 处理能力重构 hiBus
  - 若干内置服务及应用
  - 引入 HVML 应用框架

		
## 关键应用：HVCode

- 基于 HVML 的跨平台代码编辑器
- 对标 VSCode

		
## 研究课题

- 使用 HVML 实现 Nginx 网关
- 使用 HVML 实现高并发应用服务器
- ...

		
# 问题及讨论

