# HVML Language

Vincent Wei <https://github.com/VincentWei>

- HVML Specifications: <https://github.com/HVML/html-docs>
- HVML Interpreter PurC: <https://github.com/HVML/PurC>

		
## Outline

1. What's HVML?
1. Special features of HVML
1. Intersting features of HVML
1. Why we designed HVML?
1. Benefits we'v got
1. Benefits we'll get
1. Current status of HVML Community
1. Q & A

		
## What's HVML

- HVML is the acronym for `Hybrid Virtual Markup Language`
- HVML is a descriptive programming language
- HVML is a programmable markup language.
- HVML program is driven by data and events.
- HVML program outputs data and a structured document.

<p>
<img alt="HVML Logo" src="assets/hvml-logo.png" width="200">
</p>

		
## Special features of HVML

- We use modified markup syntax to write an HVML program.
- HVML uses extended JSON notation to define data, attribute values, and expressions.
- HVML introduces a dozen of verb tags, preposition attributes, and adverb attributes to define the operations and the program structure.

	
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

<!-- initialize a set variable from an object array;
     the property `id` will have unique values -->
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

<!-- initialize a set variable from the remote data source,
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

<!-- When the async. operation has done successfully,
     we will got `change:attached` event against the `users` variable -->
<observe against "users" for "change:attached" in "#user-list">
    <clear on $@ />
    <iterate on $users>
        <update on $@ to 'append' with $user_item />
    </iterate>
</observe>

<!-- Create a new timer identified by `foo` with the interval 3000ms -->
<update on $TIMERS to 'append'>
    { "id" : "foo", "interval" : 3000, "active" : "yes" }
</update>

<!-- observe the `expired:foo` event on $TIMERS -->
<observe on $TIMERS for 'expired:foo' >
    <!-- disable the timer identified by `foo` -->
    <choose on $TIMERS by "FILTER: AS 'foo'">
        <update on $? at ".active" with "no" />
    </choose>
</observe>

<!-- Bind an expression as a new variable, and observe the
     changes of the expreesion -->
<bind on $SYS.time as "rtClock" >
    <observe on $rtClock for "change">
       ...
    </observe>
</bind>
```

	
### Other features in a glance

- Data templates or document templates
- Use templates to handle exceptions
- Closures

		
## Intersting features of HVML

- Compound expression
- Operation group and executing in-place
- Coroutines
- Runners and Concurrence

	
### Compound expression

```javascript
{{
    // Try to change the working directory to `/root`.
    $SYS.cwd(! '/root') &&
        // If success, call `$FS.list` to get the all directory entries.
        $FS.list ||
            // Otherwise, print a prompt line to STDOUT `STREAM.stdout`,
            $STREAM.stdout.writelines('Cannot change directory to "/root"');
            // and change the working directory to `/`.
            $SYS.cwd(! '/' ) &&
                // If success, call `$FS.list` to get the all directory entries.
                $FS.list ||
                    // Otherwise, use `false` as the multimate result of
                    // this compound expression.
                    false
}}
```

	
### Operation group and executing in-place

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

<!-- define an operation group, which outpus an HTML fragment -->
<define as "output_html">
    <h1>HVML</h1>
    <p>$?</p>
</define>

<!-- define an operation group, which outputs a line to the STDOUT stream. -->
<define as "output_void">
    <inherit>
        $STREAM.stdout.writelines($?)
    </inherit>
</define>

<!-- This element executes one of above two operation groups in-place,
     according to the type of current target document. -->
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

<!-- output of the program:

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

		
## Why we designed HVML?

The original design goal of HVML is to allow developers who are familiar with C/C++, Python, or other programming languages to easily develop GUI applications by using Web technologies (such as HTML/SVG, DOM and CSS), instead of using JavaScript programming language in a web browser or Node.js.

- We waste a lot of time to tune the styles and layouts of GUIs when using the traditional C/C++ toolkit like MiniGUI, Gtk, or Qt.
- As a C/C++ programmer, there was no way to utilize the modern Web technologies directly.

	
### Disadvatanges of Web technologies

- The user experience of web page are always worse than the native GUIs:
   1. It's very slow to update DOM.
   1. The JavaScript engine and the web page renderer always compete for the processor; this leads to the jams of interaction.
- It is difficult to manage multiple windows by using Web technologies.
- HTTP is stateless.
- JavaScript engine always runs in the browser, it is hard to write JS extensions.

		
## Benefits We'v Got

- Use HVML to rapidly develop GUI applications based on Web technologies (HTML/SVG, DOM, CSS) in the C/C++ runtime environment.
- A cross-platform HVML interpreter: PurC.
- Use HVML as a general-purpose programming language to write scripting tools.
- A stack-based virtual machine to run HVML program.
- Decouple the interperter and the renderer.
- Flexible cross-end app frameworks.

	
### A stack-based virtual machine to run HVML programs

- We use a virtual DOM tree to represent an HVML program.
- The interpreter creates a new stack for every new HVML program instance (coroutine).
- The interperter executes every elements of vDOM from the root element in depth-first order.
- When the depth increments, there will be a new stack frame on the stack and the interpreter stores the context data in the stack frame.
- When the depth decrements, the interpreter pops the stack frame and tries to execute the next sibling element.
- All foreign elements have the universal operation: clone the attributes and contents to the current position of the target document.

	
### Four executing stages of an HVML coroutine

- `scheduled`: the coroutine is scheduled.
- `first-run`: the coroutine is in first-run stage; if there is no any observer installed, the coroutine will be switched to `cleanup` stage, otherwise to `observing` stage.
- `observing`: the coroutine wait for events and handle them.
- `cleanup`: the scheduler will send the exit status to the parent coroutine and clean up the data attached to the coroutine.

	
### Decouple the interperter and the renderer

- Use virtual DOM to avoid updating DOM frequently
- The interpreter and the renderer no longer compete for the processor.
- We can easily manage multiple windows by using HVML.
- HVML program is stateful.

	
### Cross-end app frameworks

<div class="r-stack">
  <img class="fragment" src="assets/renderer-as-thread.svg" height="600">
  <img class="fragment" src="assets/renderer-as-server.svg" height="600" />
  <img class="fragment" src="assets/renderer-as-data-bus-endpoint.svg" height="600">
  <img class="fragment" src="assets/renderer-as-client.svg" height="600">
  <img class="fragment" src="assets/renderer-headless.svg" height="600">
</div>

		
## Benefits We'll Get

- Unify CLI and GUI development.
- A Web server wrotten in HVML.
- Distributed/Decentralized deployment of an HVML app.
- Rendering to and interacting with multiple ends.
- ……

	
### Multiple ends

<p>
  <img src="assets/decentralized-and-multiple-ends.svg" height="600">
</p>

		
## Current Status of HVML Community

After one-year's development, we opened the following repositories in July, 2022:

- [HVML Specifications](https://github.com/HVML/HVML-Docs)
- [HVML Interpreter PurC](https://github.com/HVML/PurC)
- [HVML Graphics Renderer xGUI Pro](https://github.com/HVML/xGUI-Pro) and [Tailored WebKit](https://files.fmsoft.cn/hvml/webkitgtk-2.34.1-hvml-220804.tar.bz2)

	
### Statistics

- Main developers: 3
- Contributors: 10
- Stars on GitHub: 1000+
- Total LoC: 100M+
- Tests: 100+
- Test cases: 1000+
- Supported operating systems: Linux and macOS

	
### Active developement

- PurC v0.8.2 in Sep. 2022
- PurC v0.9.0 in Oct. 2022
- ……
- PurC v1.0.0 in Mar. 2023
  1. All features defined by HVML specifications.
  1. Support for Linux, macOS, and Windows.

		
## Q & A

Welcome to visit <https://github.com/HVML>

