# Samples for Best Practices of C

This repository contains the examples for my online video course:
[the Best Practices of C Language].

I use a fake project `FooBar` to show the good or bad code.

__Table of Contents__

- [Source Tree of FooBar](#source-tree-of-foobar)
- [Current Status](#current-status)
- [Coding Style](#coding-style)
   + [Indentation](#indentation)
   + [Printing messages](#printing-messages)
   + [Allocating memory](#allocating-memory)
- [Building](#building)
   + [Commands](#commands)
- [Authors and Contributors](#authors-and-contributors)
- [License](#license)


## Source Tree of FooBar

FooBar contains the skeleton of a typical C library project:

- The building system based on CMake.
- The headers and source files of the library.
- The unit test programs based on gtest.
- Some samples for the usage of the APIs.

The source tree of FooBar is illustrated as follow:

- `docs/`: The lecture slides of this course.
- `src/foobar/include/`: The global header files.
- `src/foobar/include/private`: The internal header files.
- `src/foobar/utils/`: Some basic and common utilities.
- `src/foobar/foo/`: The module of Foo.
- `src/foobar/bar/`: The module of Bar.
- `src/foobar/xxx/`: The module of XXX.
- `src/foobar/ports/`: The ports for different platforms,
    such as a POSIX-compliant systems or Windows.
- `src/foobar/bindings/`: The bindings for Python, Lua,
    and other programming languages.
- `src/wtf/`: The simplified WTF (Web Template Framework) from WebKit.
- `src/cmake/`: The cmake modules.
- `src/third-party/`: The third-party libraries, such as `gtest`.
- `src/test/`: The unit test programs.
- `tools/`: Some tools for easy development.
- `examples/`: The examples using the library.

## Current Status

This project was launched in Sept. 2021.

We welcome anybody to take part in the development and contribute your effort!

For the community conduct, please refer to [Code of Conduct](CODE_OF_CONDUCT.md).

## Coding Style

We mainly follow [Linux Kernel Coding Style], with the following exceptions.

### Indentation

We always use four spaces for indentation.

### Printing messages

### Allocating memory

## Building

### Commands

To build the source on Linux:

```
$ mkdir build
$ cd build
$ cmake .. -DPORT=Linux
$ make
```

To build the source on macOS:

```
$ mkdir build
$ cd build
$ cmake .. -DPORT=Mac
$ make
```

## Authors and Contributors

- Vincent Wei

## License

The MIT License (MIT)

Copyright © 2021 FMSoft (<https://www.fmsoft.cn>).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.


[Linux Kernel Coding Style]:https://www.kernel.org/doc/html/latest/process/coding-style.html
[the Best Practices of C Language]:https://courses.fmsoft.cn/best-practices-of-c/
