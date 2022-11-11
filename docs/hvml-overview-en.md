# HVML Overview

- HVML Specifications: <https://github.com/HVML/html-docs>
- HVML Interpreter PurC: <https://github.com/HVML/PurC>

		
## Outline

1. What's HVML?
1. Basic features of HVML
1. Intersting Features of HVML
1. Why We Designed HVML?
1. Current Status
1. Benefits We'v Got
1. Benefits We'll Get

		
## What's HVML

- HVML is the acronym for `Hybrid Virtual Markup Language`
- HVML is a descriptive programming language
- HVML is a programmable markup language.
- HVML program is driven by data and events.
- HVML program outputs data and structured document.

<p>
<img alt="HVML Logo" src="assets/hvml-logo.png" width="200">
</p>

		
## Basic features of HVML

- We use markup syntax to write a HVML program.
- HVML uses extended JSON notation to define data, attribute values, and expressions.
- HVML introduces a dozen of verb tags, preposition attributes, and adverb attributes to define an operation.

	
### Hello, world!

```hvml
<!DOCTYPE hvml>
<hvml target="void">

    $STREAM.stdout.writelines('Hello, world!')

</hvml>
```

or

```hvml
<!DOCTYPE hvml>
<hvml target="html">
  <head>
    <title>Hello, World!</title>
  </head>

  <body>
    <iterate on [0, 1, 2] >
        <p>Hello, world! -- $?</p>
    </iterate>
  </body>

</hvml>
```

	
### Descriptive

```hvml
<init as 'emptyArray' with [] />

<!-- initialize an array variable from an object array -->
<init as 'users'>
    [
        { "id": "1", "avatar": "/img/avatars/1.png", "name": "Tom",
            "region": "en_US", "age": 2 },
        { "id": "2", "avatar": "/img/avatars/2.png", "name": "Jerry",
            "region": "zh_CN", "age": 3 }
    ]
</init>

<!-- initialize an set variable from an object array -->
<init as 'users' uniquely against 'id' silently>
    [
        { "id": "1", "avatar": "/img/avatars/1.png",
            "name": "Tom", "region": "en_US" },
        { "id": "2", "avatar": "/img/avatars/2.png",
            "name": "Jerry", "region": "zh_CN" }
        { "id": "2", "avatar": "/img/avatars/2.png",
            "name": "David", "region": "zh_CN" }
    ]
</init>

<!-- initialize an set variable from the remote data source,
     but has a default data  -->
<init as 'users' from "https://foo.bar.com/users/$SYS.locale"
        uniquely against 'id' silently async >
    [
        { "id": "1", "avatar": "/img/avatars/1.png",
            "name": "Tom", "region": "en_US" },
        { "id": "2", "avatar": "/img/avatars/2.png",
            "name": "Jerry", "region": "zh_CN" }
        { "id": "2", "avatar": "/img/avatars/2.png",
            "name": "David", "region": "zh_CN" }
    ]
</init>
```

	
### Data-Driven and Event-Driven

```hvml
<init as "users" from "http://foo.bar.com/get_all_users" async />

<archetype name="user_item">
    <li class="user-item">
        <img class="avatar" src="" />
        <span></span>
    </li>
</archetype>

<ul class="user-list">
    <img src="wait.png" />
</ul>

<!-- 当我们在 users 变量上获得 `change:attached` 事件时，
     说明异步操作已完成，其数据已绑定到 users 变量上了。 -->
<observe against "users" for "change:attached" in "#user-list">
    <clear on $@ />
    <iterate on $users>
        <update on $@ to 'append' with $user_item />
    </iterate>
</observe>

<!-- 新增标识符为 `foo` 的定时器，间隔 3000 ms，激活状态 -->
<update on $TIMERS to 'append'>
    { "id" : "foo", "interval" : 3000, "active" : "yes" }
</update>

<!-- 观察标识符为 `foo` 的定时器的到期事件 -->
<observe on $TIMERS for 'expired:foo' >
    <!-- 使标识符为 `foo` 的定时器失效 -->
    <choose on $TIMERS by "FILTER: AS 'foo'">
        <update on $? at ".active" with "no" />
    </choose>
</observe>

<!-- 将一个表达式绑定为一个变量，以便观察这个表达式值的变化 -->
<bind on $SYS.time as "rtClock" >
    <observe on $rtClock for "change">
       ...
    </observe>
</bind>
```

	
### Other features

- Data templates or document templates
- Use templates to handle exceptions
- Closures

		
## Intersting Features of HVML

- Compound expression
- Operation set and executing in-place
- Coroutines
- Runners and Concurrence

	
### Compound expression

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

	
### Operation set and executing in-place

```hvml
<define as 'getDirEntries'>
    <init as 'result' with [] temp />

    <choose on $FS.opendir($?) >
        <iterate with $?.read() >
            <update on $result to 'append' with $? />
        </iterate>
    </choose>

    <return with $result />
</define>

<call on $getDirEntries with '/home/' >
    <!-- the directory entries will be in $? -->
</call>

<!-- 该元素定义了一个操作组，该操作组输出 HTML 片段。-->
<define as "output_html">
    <h1>HVML</h1>
    <p>$?</p>
</define>

<!-- 该元素定义了一个操作组，该操作组向系统的标准输出打印文本。-->
<define as "output_void">
    <inherit>
        $STREAM.stdout.writelines($?)
    </inherit>
</define>

<!-- 该元素根据当前 `hvml` 元素的 `target` 属性值就地执行不同的操作组。-->
<include with ${output_$CRTN.target} on $T.get('Hello, world!') />
```

	
### Coroutines

```
<hvml target="void">
    <body>

        <load from "#repeater" onto '_null' async />

        <inherit>
            {{
                 $STREAM.stdout.writelines("COROUTINE-$CRTN.cid: $DATETIME.time_prt: I will sleep 5 seconds");
                 $SYS.sleep(5);
                 $STREAM.stdout.writelines("COROUTINE-$CRTN.cid: $DATETIME.time_prt: I am awake.");
            }}
        </inherit>

    </body>

    <body id="repeater">

        <iterate on 0 onlyif $L.lt($0<, 5) with $EJSON.arith('+', $0<, 1) nosetotail >
            $STREAM.stdout.writelines("COROUTINE-$CRTN.cid: $DATETIME.time_prt")

            <sleep for '1s' />
        </iterate>

    </body>
</hvml>

<!--
COROUTINE-3: 2022-09-01T14:50:40+0800: I will sleep 5 seconds
COROUTINE-4: 2022-09-01T14:50:40+0800
COROUTINE-4: 2022-09-01T14:50:41+0800
COROUTINE-4: 2022-09-01T14:50:42+0800
COROUTINE-4: 2022-09-01T14:50:43+0800
COROUTINE-4: 2022-09-01T14:50:44+0800
COROUTINE-3: 2022-09-01T14:50:45+0800: I am awake.
-->
```

	
### Runners and Concurrence

```hvml
<call as 'my_task' on $collectAllDirEntriesRecursively with "/"
        within "myRunner" concurrently asynchronously />

<observe on $my_task for 'callState:success'>
    <iterate on $? in "#entries">
        <update on $@ to 'append' with $dir_entry />
    </iterate>
</observe>

<load as 'myRepeater' from 'myrepeater.hvml'
        within 'myRunner' asynchronously>
    <observe on $myRepeater for 'corState:observing'>
        <request on $myRepeater to 'echo1' >
            "How are you?"
        </request>
    </observe>
</load>
```

	
### Substitue Expression

```hvml
<bind on "$STREAM.stdout.writelines($_ARG0)" as "console" at 'puts' />

<inherit>
    $console.puts('Hello, world!')
</inherit>

<bind on $SYS.time as "rtClock" >
    <observe on $rtClock for "change">
       ...
    </observe>
</bind>

<bind on $MATH.div(1.0, $MATH.sqrt($_ARG0)) as 'myConst'
        against 'reciprocal_of_sqrt' constantly />

<choose on $myConst.reciprocal_of_sqrt_const(2.0) >
    ...
</choose>
```

		
## Why We designed HVML?

	
### 我自己的切身体会

- 开发效率的提升，对开发者意义重大
   1. MiniGUI、Gtk、Qt 采用的是 C/C++ 接口，开发者的时间浪费在对大量界面细节的调整上。
   1. Web 前端技术广泛使用 HTML/CSS 等描述式语言，用于界面样式、布局以及动画的定义，大大提高了开发效率。
- 开发者对任何提升开发效率的技术和工具，持有天然的兴趣和热情。

		
## Current Status

- 2022.07.31 在 GitHub 上开源，含六个软件仓库/软件包
   1. [解释器 PurC](https://github.com/HVML/PurC)
   1. [数据获取器 PurC Fetcher](https://github.com/HVML/PurC-Fetcher)
   1. [字符渲染器 PurC Midnight Commander](https://github.com/HVML/PurC-Midnight-Commander)
   1. [DOM 布局器 DOM Ruler](https://github.com/HVML/DOM-Ruler)
   1. [图形渲染器 xGUI Pro](https://github.com/HVML/xGUI-Pro)
   1. [定制 WebKit 引擎（软件包）](https://files.fmsoft.cn/hvml/webkitgtk-2.34.1-hvml-220804.tar.bz2)

	
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

		
## Benefits We'v Got

- 起先，我们的目标是让熟悉 C/C++ 或其他编程语言的开发人员可以通过 HVML 使用 Web 前端技术轻松开发 GUI 应用程序，而不是使用 Electron 这样的庞然大物。
- 现在，我们不光实现了这一目标，而且还将 HVML 实现为一种通用的、描述式编程语言。

		
## Benefits We'll Get

		

Q & A


	
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
    with "$hvml.name 来自 $hvml.birthland，年龄 $hvml.age 岁" />
```

		
## 技术特征(5/8)：栈式虚拟机

- HVML 程序被解析后，会生成代表该程序结构的 vDOM 树，解释器从根元素开始以深度优先的顺序执行 vDOM 上的每个元素。
- 每执行一个子孙元素，虚拟机的执行栈上会压入一个新的栈帧，并在栈帧中保存上下文数据。
- 解释器执行完叶子元素之后，会弹出最后的栈帧，此时释放上下文数据，上一个栈帧变为最后的栈帧，然后尝试执行该栈帧对应元素的下个兄弟元素。
- 所有的外部元素具有一个统一的操作：克隆该元素的属性和内容到当前的目标文档位置并隐式调整 HVML 程序的上下文数据。

	
### 闭包

- HVML 的静态命名变量绑定到指定的 vDOM 元素上，其作用域为该元素定义的 vDOM 子树
- HVML 的临时命名变量保存在栈帧中，且允许访问前置栈帧中的数据

```hvml
<archetype name="dir_entry">
    <item class="$?.type">Name: $?.name</item>
</archetype>

<define as 'fillDirEntries'>
    <choose on $? by "CLASS: CDirEntries">
        <iterate on $? by "RANGE: FROM 0">
            <update on $@ to 'append" with $dir_entry />
        </iterate>
    </choose>
</define>

<listbox id="entries">
    <!-- 定义一个新的 `dir_entry` 模板，
         该模板在文件名之前显示 `/home/`（路径）前缀 -->
    <archetype name="dir_entry">
        <item class="$?.type">/home/$?.name</item>
    </archetype>

    <!-- `fillDirEntries` 操作组将使用上面这个新的 `dir_entry` 模板 -->
    <include with $fillDirEntries on '/home' in '#entries' />
</listbox>
```

	
### 操作组

- 操作组由 `define` 元素定义，是一个 vDOM 子树，可动态装载并覆盖

在 `/module/html/listitems.hvml` 中定义一个展示数组成员的操作组：

```hvml
<ol>
    <iterate on $?>
        <li>$?</li>
    </iterate>
</ol>
```

默认的操作组向标准输出流写入数组成员：

```hvml
<define as 'listitems' from '/module/$CRTN.target/listitems.hvml'>
    <inherit>
        $STREAM.stdout.writelines($?)
    </inherit>
</define>

<include with $listitems on ['Line #1', 'Line #2'] />
```

	
### 调用和就地执行

- 调用操作组和常见编程语言中的函数调用类似

```hvml
<define as 'getDirEntries'>
    <init as 'result' with [] temp />

    <choose on $FS.opendir($?) >
        <iterate with $?.read() >
            <update on $result to 'append' with $? />
        </iterate>
    </choose>

    <return with $result />
</define>

<call on $getDirEntries with '/home/' >
    <!-- the directory entries will be in $? -->
</call>
```

	
- 就地执行相当于复制指定的操作组到当前的位置执行

```hvml
<!-- 该元素定义了一个操作组，该操作组输出 HTML 片段。-->
<define as "output_html">
    <h1>HVML</h1>
    <p>$?</p>
</define>

<!-- 该元素定义了一个操作组，该操作组向系统的标准输出打印文本。-->
<define as "output_void">
    <inherit>
        $STREAM.stdout.writelines($?)
    </inherit>
</define>

<!-- 该元素根据当前 `hvml` 元素的 `target` 属性值就地执行不同的操作组。-->
<include with ${output_$CRTN.target} on $T.get('Hello, world!') />
```

	
### 使用模板处理异常

```hvml
<ul class="user-list">
    <iterate on="$users" by="CLASS: IUser">
        <update on="$@" to="append" with="$user_item" />
        <except type="NoData">
            <img src="wait.png" />
        </except>
        <except type="NotIterable">
            <p>Bad user data!</p>
        </except>
    </iterate>
</ul>
```

	
### 捕获异常

```hvml
<init as="locales">
  {
      "zh_CN" : {"se_name" : "Baidu",
          "se_url": "https://www.baidu.com", "se_title": "百度" },
      "zh_TW" : {"se_name" : "Bing",
          "se_url": "https://www.bing.com", "se_title": "必应" }
  }
</init>

<footer id="the-footer">
    <p><a href="" title=""></a></p>
</footer>

<choose on "$locales" in "#the-footer" by "KEY: AS '$SYS.locale'">
    <update on "p > a" at "textContent attr.href attr.title"
            with ["$?.se_name", "$?.se_url", "$?.se_title"] />
    <catch for `NoData`>
        <update on "p" at "textContent"
                with 'You forget to define the $locales/$global variables!' />
    </catch>
    <catch for "NoSuchKey">
        <update on "p > a" at "textContent attr.href attr.title"
                with ["Google", "https://www.google.com", "Google"] />
    </catch>
    <catch for "*">
        <update on 'p' at "textContent"
                with 'Bad $locales/$global data!' />
    </catch>
</choose>
```

		
## 技术特征(6/8)：事件驱动

- 通过 `observe` 元素观察特定数据、渲染器以及静态命名变量上的事件


	
### 程序的执行阶段和执行状态

一个正确解析并装载的 HVML 程序以协程的形式运行。HVML 定义协程有如下四种执行阶段（execution stage）：

- 已排（scheduled）：已安排阶段。当调度器选择处于该阶段的协程进入就绪状态时，进入首轮执行阶段。
- 首轮（first-run）：首轮执行阶段。当首轮执行阶段完成后，若该协程没有观察任何事件，则进入清理阶段；若注册有观察者，则进入事件循环执行阶段。
- 观察（observing）：事件循环执行阶段。
- 清理（cleanup）：清理阶段。在清理阶段，调度器将协程的退出或终止状态发送给父协程（若有）。

	
除了协程正常执行进入清理阶段之外，一个 HVML 协程可能由于异常而终止。故而我们使用如下术语区分一个协程的退出或终止状态：

- 退出（exited）：隐式退出或者主动退出；自然执行完所有的元素，且没有注册任何观察者；或者执行 `exit` 动作元素主动退出。协程退出时，最顶栈帧中保存的结果数据将被作为协程的结果数据。
- 终止（terminated）：由于错误或者未捕获的异常而终止。协程被终止时，结果数据为异常名称。

	
在正常调度过程中，一个 HVML 协程有如下三种执行状态（execution state）：

- 就绪（ready）：该协程正在等待执行，调度器将按次序选择执行该协程。
- 执行（running）：该协程正在执行。
- 暂停（stopped）：该协程被停止等待特定唤醒条件的到来，比如子协程退出、主动休眠到期、异步数据获取器返回数据、并发调用返回结果、调试器要求继续执行等。当设定的唤醒条件到来时，调度器设置该协程的状态为就绪（ready）。

		
## 技术特征(8/8)：动态性

- 变量名、表达式、操作组、程序，皆可动态生成。
- 变量可被移除。
- 替身表达式。

	
### 装载动态生成的程序

```hvml
<init as="request">
    {
        hvml: '<hvml target="html"><body><h1>$REQ.text</h1><p>$REQ.hvml</p></body></hvml>',
        text: "Hello, world!",
        _renderer: {
            title: 'Hello, world!',
            class: 'hello',
            style: 'with:200px;height:100px',
        },
    }
</init>

<load on "$request.hvml" onto "hello@main" >
    $request
</load>
```

	
### 替身表达式

```hvml
<bind on "$STREAM.stdout.writelines($_ARG0)" as "console" at 'puts' />

<inherit>
    $console.puts('Hello, world!')
</inherit>


<bind on $MATH.div(1.0, $MATH.sqrt($_ARG0)) as 'myConst' at 'reciprocal_of_sqrt' constantly />

<choose on $myConst.reciprocal_of_sqrt_const(2.0) >
    ...
</choose>
```

		
# 演示

- [普通计算器](https://www.bilibili.com/video/BV1qe411T7Yc)
- [任意精度计算器](https://www.bilibili.com/video/BV1DG4y1z7Ki)
- [行星共振](https://www.bilibili.com/video/BV1UB4y1n7r3)

	
## 普通计算器

<iframe src="//player.bilibili.com/player.html?aid=260592546&bvid=BV1qe411T7Yc&cid=834585020&page=1"
        width="800" height="450" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

	
## 任意精度计算器

<iframe src="//player.bilibili.com/player.html?aid=815524776&bvid=BV1DG4y1z7Ki&cid=834571892&page=1"
        width="800" height="450" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

	
## 行星共振

<iframe src="//player.bilibili.com/player.html?aid=603105372&bvid=BV1UB4y1n7r3&cid=834582404&page=1"
        width="800" height="450" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

		
## 典型 HVML 程序

- 计算两个正整数的最大公约数
- 生产者消费者
- 素数筛

	
## 计算两个正整数的最大公约数

```hvml
# RESULT: [3L, 11L, 1L]

<!-- 最大公约数 -->

<!DOCTYPE hvml>
<hvml target="void">

    <init as 'result' with [] temp />

    <define as "calcGreatestCommonDivisor">
        <test with $L.or($L.le($x, 0), $L.le($y, 0)) >
            <return with undefined />
        </test>

        <!-- 我们使用复合表达式来获得与 C 语言 `(x > y) ? x : y`
             语句相同的结果 -->
        <init as 'big'
                with {{ $L.gt($x, $y) && $x || $y }} temp />
        <init as 'small'
                with {{ $L.lt($x, $y) && $x || $y }} temp />

        <!-- 如果 `$big` 和 `$small` 互质，
             则直接返回 `$small` 作为结果 -->
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
# RESULT: [ 2L, 3L, 5L, 7L, 11L, 13L, 17L, 19L, 23L, 29L, 31L, 37L, 41L, 43L, 47L, 53，59L, 61L, 67L, 71L, 73L, 79L, 83L, 89L, 97L ]

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

		
## 解释器模块框图

```
 -----------------------------------------------------------------------------
|                                                                             |
|  Parsers                                                                    |
|            --------------------------------------------------------         |
|           | HVML Parser | HTML Parser | XGML Parser |  XML Parser  |        |
|            --------------------------------------------------------         |
|                              |  eJSON Parser  |                             |
|                               ----------------                              |
|                                                                             |
|  Tree Constructors                                                          |
|            --------------------------------------------------               |
|           |  vDOM Tree Constructor |  eDOM Tree Constructor  |              |
|            --------------------------------------------------               |
|               | Variant Creation Model Tree Constructor |                   |
|                -----------------------------------------                    |
|                                                                             |
|  HVML Interpreter Module                                                    |
|                                                                             |
|                 ---------------------------------------------               |
|                |  Runner Mgt. | Coroutine Mgt. |  Stack Mgt. |              |
|            ---------------------------------------------------------        |
|           |  Scheduler | Evaluation of Variant Creation Model Tree  |       |
|            ---------------------------------------------------------        |
|                       |  Mgt. of Context Variables  |                       |
|             -----------------------------------------------------           |
|            | Mgt. of Builtin Variables | Mgt. of Bound Variables |          |
|        ---------------------------------------------------------------      |
|       | Mgt. of Event Listeners | Bridging between eDOM and Renderer  |     |
|        ---------------------------------------------------------------      |
|                                                                             |
|  Dynamic Variant Objects                                                    |
|                                                                             |
|          ------------------------------------------------------------       |
|         | Builtin DVObjs: SYS, STR, STREAM, L, T, TIMERS, EJSON, ... |      |
|         --------------------------------------------------------------      |
|        | Dynamic Variant Object Loader | Ext. DVObjs: FS, FILE, MATH  |     |
|         --------------------------------------------------------------      |
|                                                                             |
|  Executors                                                                  |
|                                                                             |
|           ---------------------------------------------------------         |
|          | Builtin Executors: RANG, KEY, FILTER, TRAVEL, SQL, ... |         |
|           ---------------------------------------------------------         |
|               | Support of External Executors: FUNC and CLASS |             |
|                -----------------------------------------------              |
|                                                                             |
|  Data Fetchers                                                              |
|                                                                             |
|           ----------------------------------------------------              |
|          | local data fetcher: 'file:', 'lsql:', 'lcmd:', ... |             |
|           ----------------------------------------------------              |
|             | Support of Remote Data Fetcher (PurC Fecther) |               |
|              -----------------------------------------------                |
|                                                                             |
|  C/C++ Utilities/Helpers                                                    |
|                                                                             |
|             ------------------------------------------------                |
|            | UTF-8 Encoding/Decoding | Atomic Strings | ... |               |
|             ------------------------------------------------                |
|                                                                             |
 ----------------------------------------------------------------------------- 
```

		
## 应用框架

- 程序逻辑和界面渲染/交互细节的解耦
- 五种架构
   1. 渲染器作为服务器
   1. 渲染器作为数据总线端点
   1. 渲染器作为独立线程（内置）
   1. 渲染器作为客户端（云应用，远程应用）
   1. 无头渲染器（普通命令行程序）

	
### 渲染器作为服务器

![渲染器作为服务器](assets/renderer-as-server.svg)

	
### 渲染器作为数据总线端点

![渲染器作为数据总线端点](assets/renderer-as-data-bus-endpoint.svg)

	
### 渲染器作为独立线程

![渲染器作为独立线程](assets/renderer-as-thread.svg)

	
### 渲染器作为客户端

![渲染器作为客户端](assets/renderer-as-client.svg)

	
### 无头渲染器

![无头渲染器](assets/renderer-headless.svg)

		
## 应用场景

- 跨平台移固融合应用开发（万物互联）
- 桌面应用（Electron）
- 用于 C/C++/Rust 的内嵌胶水语言（Lua）
- 独立脚本语言（Python）
- Web服务器网关（PHP）
- 高并发服务器（Go/Erlang）

		
## 现状及路线图

- 2022.07.31 在 GitHub 上开源，含六个软件仓库/软件包
   1. [解释器 PurC](https://github.com/HVML/PurC)
   1. [数据获取器 PurC Fetcher](https://github.com/HVML/PurC-Fetcher)
   1. [字符渲染器 PurC Midnight Commander](https://github.com/HVML/PurC-Midnight-Commander)
   1. [DOM 布局器 DOM Ruler](https://github.com/HVML/DOM-Ruler)
   1. [图形渲染器 xGUI Pro](https://github.com/HVML/xGUI-Pro)
   1. [定制 WebKit 引擎（软件包）](https://files.fmsoft.cn/hvml/webkitgtk-2.34.1-hvml-220804.tar.bz2)

	
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

	
- 2023.07 发布 HVML 规范 2.0
  - 面向对象
  - 基于汉语的 HVML 语法
- 2023.12 发布 PurC 2.0
  - 符合 HVML 规范 2.0
  - 第三方组件的分发及包管理
  - 第三方动态对象的分发及包管理
  - 支持 iOS、Android

		
## 关键应用：PurC + xGUI Pro

- 定位：跨平台桌面应用开发
- 对标：Electron
- 跨平台：Linux、macOS、Windows
- 目标：30% 以上市占率

		
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

		
## 核心技术

- 参数化 eJSON 表达式
- HVML 解释器
- HVML 调试器
- HTML 渲染器
- 解释器到渲染器的 PURCMC 协议
- 数据总线协议

		
## 研究课题

- 使用 HVML 实现 Nginx 网关
- 使用 HVML 实现高并发应用服务器
- 自举
- ...

		
# 问题及讨论

