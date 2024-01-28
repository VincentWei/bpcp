# 使用说明

[TOC]

【注】本文摘自魏永明著《C语言最佳实践》第5章“善用构建系统生成器”。略有修改。

## 整体结构和克隆步骤

该 CMake 项目模板的初始名称为 FooBar，其中包含了《C语言最佳实践》课程用到的程序清单、示例程序等。该项目模板的核心内容包括如下几个部分：

- 根目录的 `CMakeLists.txt` 文件。这是项目的主 CMake 脚本。
- `source/cmake/` 子目录中的 CMake 脚本。这些脚本中以 `Golbal` 打头的文件，定义了若干方便使用的 CMake 宏，而 `Find` 打头的脚本，主要用于查找当前系统中是否安装有特定的软件包（统称为“软件包”）。
- `source/wtf/` 目录树中来自 WTF（WebKit Template Framework，WebKit 模板框架）的头文件。这些头文件的主要作用用于检测处理器架构以及编译器特性，并进一步封装成易于使用的宏。

应用于实际项目时，可复制以上的文件及子目录树到自己的项目根目录下，并使用真实的项目名称全程替换其中的 `FooBar`、`FOOBAR` 和 `foobar`。

假定我们计划新建一个 C、C++ 项目，名为 `HelloWorld`，则在 Linux 系统上，复制上述目录及文件，并在源代码树的根目录下使用如下命令行：

```console
$ sed -i 's/FooBar/HelloWorld/g' `grep FooBar * -rl`
$ sed -i 's/FOOBAR/HELLOWORLD/g' `grep FOOBAR * -rl`
$ sed -i 's/foobar/helloworld/g' `grep foobar * -rl`
```

并将 `source/cmake/target` 目录下的 `FooBar.cmake` 文件更名为实际的项目名称，如 `HelloWorld.cmake`。

按照惯例，应在 `source/` 目录中包含项目的头文件及源文件，比如针对 FooBar 项目，其源文件包含在 `source/foobar` 目录下；而 HelloWorld 项目的源文件应包含在 `source/helloworld` 目录下。还可以根据项目规模大小，在 `source/` 目录下创建多个子目录用于不同的模块。

假定 HelloWorld 项目只有一个头文件 `hello.h`，还有两个 C 源文件 `hello.c` 及 `main.c`。头文件被置于 `source/helloworld/include/` 目录下。第一个源文件将被编译为函数库，因此单独置于 `source/helloworld/lib/` 目录下；第二个源文件调用函数库中的函数，最终被编译为可执行程序，因此置于 `source/helloworld/bin/` 目录下。则按以上步骤处理后，HelloWorld 项目的目录树大致如下（因篇幅所限，部分文件被省略）：

```console
$ tree .
.
├── CMakeLists.txt
└── source
    ├── cmake
    │   ├── FindSQLite3.cmake
    │   ├── GlobalCommon.cmake
    │   ├── GlobalCompilerFlags.cmake
    │   ├── OptionsCommon.cmake
    │   ├── OptionsLinux.cmake
    │   ├── OptionsMac.cmake
    │   ├── ...
    │   ├── target
    │   │   └── HelloWorld.cmake
    │   ├── VersioningUtils.cmake
    │   └── WinTools.make
    ├── helloworld
    │   ├── bin
    │   │   └── main.c
    │   ├── include
    │   │   └── hello.h
    │   └── lib
    │       └── hello.c
    └── wtf
        ├── CMakeLists.txt
        ├── MyMacros.cmake
        └── wtf
            ├── CMakeLists.txt
            ├── Compiler.h
            ├── ExportMacros.h
            ├── nothing.c
            ├── PlatformCallingConventions.h
            ├── ...
            ├── Platform.h
            ├── PlatformHave.h
            └── TLSKeyword.h.in
```

然后依次在 `source/` 目录下以及 `source/helloworld` 及其子目录下创建 `CMakeLists.txt` 文件。

`source/` 目录下的 `CMakeLists.txt` 文件最为简单，其主要内容为：

```cmake
add_subdirectory(wtf)
add_subdirectory(helloworld)
```

以上内容用于指引 cmake 命令进入对应的子目录继续处理。

类似地，`source/helloworld` 目录下的 `CMakeLists.txt` 文件也不复杂，其主要内容为：

```cmake
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(bin)
```

在 `source/helloworld/include` 目录下，我们需要 CMake 将 `hello.h` 安装到系统中，故而其 `CMakeLists.txt` 文件稍微复杂一些：

```cmake
# 列出要安装到系统中的头文件
set(HelloWorld_FRAMEWORK_HEADERS
    "${HELLOWORLD_DIR}/include/hello.h"
)

set(HelloWorld_INSTALLED_HEADERS ${HelloWorld_FRAMEWORK_HEADERS})

# 安装 HelloWorld_FRAMEWORK_HEADERS 变量定义的所有头文件到指定的系统路径
install(FILES ${HelloWorld_INSTALLED_HEADERS}
        DESTINATION "${HEADER_INSTALL_DIR}/helloworld"
)
```

在 `source/helloworld/lib` 目录下，我们需要 CMake 将 `hello.c` 编译为函数库并安装到系统中，故而其 `CMakeLists.txt` 文件要更复杂一些。见程序清单 1。

__程序清单1__  
用于构建共享库的 CMake 脚本

```cmake
set_property(DIRECTORY . PROPERTY FOLDER "HelloWorld")

# 包含 CMake 构建体系的入口脚本
include(GlobalCommon)

# 设定编译源文件时的头文件搜索路径
set(HelloWorld_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${HELLOWORLD_DIR}"
    "${HELLOWORLD_DIR}/include"
    "${HelloWorld_DERIVED_SOURCES_DIR}"
    "${FORWARDING_HEADERS_DIR}"
)

# 列出所有参与编译的源文件
list(APPEND HelloWorld_SOURCES
    "${HELLOWORLD_DIR}/lib/hello.c"
)

set(HelloWorld_LIBRARIES)
set(HelloWorld_INTERFACE_LIBRARIES HelloWorld)
set(HelloWorld_INTERFACE_INCLUDE_DIRECTORIES ${HelloWorld_PRIVATE_FRAMEWORK_HEADERS_DIR})

# 使用全局宏声明一个框架（函数库）
HELLOWORLD_FRAMEWORK_DECLARE(HelloWorld)
HELLOWORLD_INCLUDE_CONFIG_FILES_IF_EXISTS()

HELLOWORLD_WRAP_SOURCELIST(${HelloWorld_SOURCES})

HELLOWORLD_COPY_FILES(HelloWorld_CopyPrivateHeaders
    DESTINATION ${HelloWorld_PRIVATE_FRAMEWORK_HEADERS_DIR}/helloworld
    FILES ${HelloWorld_PRIVATE_FRAMEWORK_HEADERS}
    FLATTENED
)
list(APPEND HelloWorld_INTERFACE_DEPENDENCIES HelloWorld_CopyPrivateHeaders)
if (NOT INTERNAL_BUILD)
    add_dependencies(HelloWorld_CopyPrivateHeaders HelloWorld)
endif ()

# 根据以上设定定义框架（函数库）的源文件以及目标名称等。
# 其中会用到 source/cmake/target/HelloWorld.cmake 定义的目标名称，
# 这里是 HelloWorld::HelloWorld
HELLOWORLD_COMPUTE_SOURCES(HelloWorld)
HELLOWORLD_FRAMEWORK(HelloWorld)
HELLOWORLD_FRAMEWORK_TARGET(HelloWorld)

if (${HelloWorld_LIBRARY_TYPE} MATCHES "SHARED")
    set_target_properties(HelloWorld PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
    install(TARGETS HelloWorld DESTINATION "${LIB_INSTALL_DIR}")
endif ()

if (MSVC)
    set_target_properties(HelloWorld PROPERTIES
        COMPILE_PDB_NAME ${HelloWorld_OUTPUT_NAME}
    )
endif ()

# 安装 HelloWorld 目标到系统函数库路径，默认是 /usr/local/lib
install(TARGETS HelloWorld
        DESTINATION "${LIB_INSTALL_DIR}/"
)
```

在 `source/helloworld/bin` 目录下，我们需要 CMake 将 `main.c` 编译后连接上面生成的函数库生成可执行程序，并安装到系统中，故而其 `CMakeLists.txt` 文件也更为复杂一些。见程序清单 2。

__程序清单2__  
构建可执行程序的 CMake 脚本

```cmake
# 包含 CMake 构建体系的入口脚本
include(GlobalCommon)
# 包含 Helloworld::Helloworld 目标（即函数库）的声明信息
include(target/HelloWorld)

# 声明 helloworld 可执行程序
HELLOWORLD_EXECUTABLE_DECLARE(helloworld)

# 列出编译源文件时的头文件搜索路径
list(APPEND helloworld_PRIVATE_INCLUDE_DIRECTORIES
    "${HELLOWORLD_DIR}/include"
)

# 生成 helloworld 可执行程序相关的变量
HELLOWORLD_EXECUTABLE(helloworld)

# 列出编译到可执行文件中的所有源文件
list(APPEND helloworld_SOURCES
    main.c
)

# 设定可执行程序需要连接的函数库
set(helloworld_LIBRARIES
    HelloWorld::HelloWorld
)

# 设定可执行程序的生成位置：构建路径的 bin/ 子目录下
set_target_properties(helloworld PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 生成构建 helloworld 可执行程序相关的 CMake 指令
HELLOWORLD_COMPUTE_SOURCES(helloworld)
HELLOWORLD_FRAMEWORK(helloworld)

# 设定可执行程序的安装路径，默认为 /usr/local/bin
install(TARGETS helloworld DESTINATION "${EXEC_INSTALL_DIR}/")
```

在做好以上准备工作后，在源文件目录树的根目录，创建 `build` 子目录，进入该子目录并执行如下命令即可完成该项目的构建及安装：

```console
$ cmake .. -DPORT=Linux
$ make
$ sudo make install
```

其中 `cmake` 的命令行选项 `-DPORT=Linux` 指定了项目的目标平台，通常使用操作系统名称或者目标图形平台的名称，如 GTK、MiniGUI。在该命令行中，通过 `-DCMAKE_BUILD_TYPE` 选项，可指定构建类型，默认取 `RelWithDebInfo`。现在观察构建目录下的内容，会看到如下文件：

- `cmakeconfig.h` 文件，其中包含了上节提到的功能测试宏，也就是 CMake 脚本检测指定函数、函数库等的结果。
- `bin/helloworld` 文件，这是构建脚本生成的可执行程序。通常，CMake 会将构建生成的 `helloworld` 可执行程序置于构建目录下的 `source/helloworld/bin/` 目录中，我们通过修改这一默认行为以方便我们在构建目录下执行这个可执行程序。
- `DerivedSources/` 目录，其中可包含构建系统自动生成的头文件或者源文件，或者构建系统从源代码目录中复制到这里的内部使用的头文件。比如其中包含了来自 WTF 的头文件。这可以方便开发者在构建大型项目时，不需要将内部使用的头文件安装到系统中便可让项目中的其他模块使用这些头文件。这也是为什么在构建函数库或可执行程序时，要指定 `${FORWARDING_HEADERS_DIR}` 作为头文件搜索路径的原因。

通常，可在源代码目录中定义一个全局的 `config.h` 头文件中，在其中包含 `cmakeconfig.h` 以及 WTF 提供的头文件，以方便代码的编写。全局 `config.h` 头文件的内容如下：

```c
#if defined(HAVE_CONFIG_H) && HAVE_CONFIG_H && defined(BUILDING_WITH_CMAKE)
#include "cmakeconfig.h"
#endif

#include <wtf/Platform.h>
#include <wtf/Assertions.h>
#include <wtf/ExportMacros.h>
```

然后，在所有的源文件中包含这一全局的头文件，之后就可以在源程序中使用 `cmakeconfig.h` 中定义的功能测试宏以及 WTF 头文件中定义的辅助工具宏了。

接下来看看这套 CMake 构建体系提供给我们的其他便利设施。

## 多平台支持

作为著名的开源浏览器引擎，WebKit 为多个平台提供支持，其中包括 Linux、macOS 和 Windows 平台。如何针对多个平台有效地组织和管理源文件，尤其是大型项目的源文件，则主要取决于构建体系。如前所述，本节介绍的 CMake 构建体系模板来自 WebKit，故而也继承了 WebKit 构建体系对多平台的支持能力。

这里的平台概念，可以看成是操作系统，也可以看成是项目使用的不同的底层图形系统，比如在 Linux 上，可以将不同的底层图形界面支持系统定义为项目的平台，如 MiniGUI、GTK 或 Qt。这取决于我们如何定义平台，而这往往和项目本身有关。本节的例子，将操作系统作为平台看待。

有时候，平台之间的差异很小，通过一些条件编译就能解决。比如 Linux 和 macOS 上的基础函数库，只有一些细微的差异，通过测试某些头文件、函数或者结构体成员是否存在，生成相应的功能测试宏即可。而有时候平台之间的差异会非常大，比如 Linux/macOS 和 Windows 相比，相同的接口占比只有 10% 不到，这种情况下，再采用条件编译就会让代码变得很难管理和维护。这时，我们通常会为不同的平台单独维护一个或者多个源文件，甚至单个或者多个源文件目录。然后通过构建时指定的平台名称，指引构建系统生成器根据不同的平台来生成包含不同源文件的构建描述文件。

本节介绍的构建体系模板为上述处理提供了非常友好的支持。

首先，通过源文件根目录下的 `CMakeLists.txt` 文件定义平台名称：

```cmake
set(ALL_PORTS
    Linux
    Mac
    Windows
    AppleWin
)
set(PORT "NOPORT" CACHE STRING "choose which HelloWorld port to build (one of ${ALL_PORTS})")
```

上述代码使用的术语 Port，意指“移植”，在本节提及的构建体系中，用于区分不同的平台。

其次，在 `source/cmake/` 目录下，使用 `Options<Port>.cmake` 文件为特定平台定义特殊的 CMake 配置指令。比如针对 Linux 平台，构建体系模板中包含有 `OptionsLinux.cmake` 文件，其中定义有仅针对 Linux 平台执行的 CMake 配置指令，比如检测平台特有的第三方函数库，设定平台特有的功能选项，或者依据编译工具链设定适当的安装路径前缀等。类似的文件还有 `OptionsMac.cmake`，其中定义有仅针对 macOS 平台执行的 CMake 配置指令。该目录下还有一个 `OptionsCommon.cmake` 文件，其中包含了可被所有平台共用的 CMake 配置指令。

再次，在项目真正的源文件目录下，使用 `Platform<Port>.cmake` 文件为特定平台定义特殊的 CMake 配置指令。和上面类似，本节所述的构建体系，会对针对当前平台，自动检查对应的 `Platform<Port>.cmake` 文件是否存在，若存在，则会执行其中的 CMake 配置指令。就前述针对不同平台使用不同源文件或者源文件目录的情形，使用这一机制可以非常方便地解决。举个例子，假定上个小节中的 `HelloWorld` 项目要使用一个函数，但该函数在不同的平台上有不同的实现方式。为此，针对不同的平台，可以有不同的函数实现，如 `foo-linux.c`、`foo-macos.c`、`foo-windows.c` 等，我们只要在 `source/helloworld/lib` 目录下，分别创建 `PlatformLinux.cmake`、`PlatformMac.cmake`、`PlatformWindows.cmake` 这三个文件，并在其中额外指定要参与编译的源文件即可。比如下面是针对 Linux 平台的 `PlatformLinux.cmake` 文件内容：

```cmake
# 将 foo-linux.c 追加到 HelloWorld_SOURCES 变量中
list(APPEND HelloWorld_SOURCES
    "${HELLOWORLD_DIR}/lib/foo-linux.c"
)
```

显然，通过本节介绍的 CMake 构建体系，开发者可以很方便地组织和管理项目对多个平台的支持。

## 检测系统头文件、函数或结构体成员

我们知道，不同的操作系统提供的基础库会存在较大的差异，这些差异主要体现在头文件、函数以及结构体的细节上。通过如下几个 CMake 宏，可以很方便地检测目标平台是否提供有某些头文件、函数或者检查结构体的成员：

1. `HELLOWORLD_CHECK_HAVE_INCLUDE`：用于检测指定的头文件，并生成对应的功能检测宏。
1. `HELLOWORLD_CHECK_HAVE_FUNCTION`：用于检测指定的函数，并生成对应的功能检测宏。
1. `HELLOWORLD_CHECK_HAVE_SYMBOL`：用于检测指定的符号，并生成对应的功能检测宏。
1. `HELLOWORLD_CHECK_HAVE_STRUCT`：用于检测指定的结构体成员，并生成对应的功能检测宏。

比如，当我们需要检测当前平台是否具有 `syslog.h` 头文件时，可在 `source/cmake/OptionsCommon.cmake` 文件或者平台特定的 `source/cmake/Options<PortName>.cmake` 文件中使用如下的宏：

```cmake
HELLOWORLD_CHECK_HAVE_INCLUDE(HAVE_SYSLOG_H syslog.h)
```

如果 CMake 构建体系检测到当前系统提供有 `syslog.h` 头文件，则会在 `cmakeconfig.h`头文件中定义 `HAVE_SYSLOG_H` 宏为 1，否则定义这个宏为 0。对应的，在构建体系的其他描述文件中，也可通过检测 `HAVE_SYSLOG_H` 变量来判断当前系统中是否存在这个头文件：

```cmake
if (HAVE_SYSLOG_H)
    ...
endif()
```

用于检测函数或者符号的宏具有类似的用法，因此不再赘述。但用于检测结构体的宏要稍微复杂一些。比如在 Linux 和 macOS 操作系统上，用于文件统计信息的 `struct stat` 结构体存在一些细微差别。为此，可以使用如下的宏指令来检测是否具有指定的结构体成员：

```cmake
HELLOWORLD_CHECK_HAVE_STRUCT(HAVE_STAT_BIRTHTIME "struct stat" st_birthtime sys/stat.h)
```

上面的 CMake 宏将检测 `struct stat` 中是否存在 `st_birthtime` 成员，而 `sys/stat.h` 用于指定定义这个结构体的头文件。这个结构体成员在 macOS 上有定义，在 Linux 上没有定义。故而，在 Linux 平台上，该构建体系最终会在 `cmakeconfig.h` 头文件中生成如下的功能测试宏：

```c
#define HAVE_STAT_BIRTHTIME 0
```

## 查找第三方软件包

如前所述，`source/cmake` 中包含有若干 `Find` 打头的 CMake 脚本。这些脚本用于查找目标系统当中是否安装有指定的第三方软件包。通常，CMake 会包含一些常见第三方软件包的查找脚本，这些脚本会随 CMake 一并安装到系统当中。比如在作者的 Linux 系统上，这些查找脚本包含在 `/usr/share/cmake-3.26/Modules/` 目录下，其中包含有查找 Python3、Lua、Perl、GTK 等软件组件的脚本，分别命名为 `FindPython3.cmake`、`FindLua.cmake`、`FindPerl.cmake` 以及 `FindGTK.cmake`。

在项目的 CMake 描述文件或者脚本中，我们可以使用下面的 CMake 指令来调用这些脚本：

```cmake
find_package(Python3 REQUIRED)
```

使用上面的 CMake 指令时，因为指定 Python3 为必需（REQUIRED），因此，若找不到 Python3，则构建在配置阶段就会失败。

当需要查找的软件包没有对应的标准查找脚本时，则可以在 `source/cmake` 下编写自己的查找脚本，并使用 `find_package` 指令来查找该软件包。作为示例，本节所提的 CMake 构建体系在其 `source/cmake` 目录中包含有查找 Linux 常用函数库 Glib 的查找脚本，名为 `FindGLIB.cmake`。读者可以模仿该查找脚本实现自己的软件包查找脚本。

作为惯例，查找脚本在查找到对应的软件组件之后，会使用统一的命名规范定义若干 CMake 全局变量。以 Glib 为例：

- `GLIB_FOUND`：若定义有该变量，则表示找到 Glib。
- `GLIB_INCLUDE_DIRS`：该变量包含有 Glib 头文件搜索路径。
- `GLIB_LIBRARIES`：该变量包含有 Glib 的链接库。

在构建目标时，可以使用上述变量将头文件搜索路径以及链接库添加到对应的构建选项中。比如：

```cmake
# 添加 GLIB 的头文件搜索路径
set(HelloWorld_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${HELLOWORLD_DIR}"
    "${HELLOWORLD_DIR}/include"
    "${HelloWorld_DERIVED_SOURCES_DIR}"
    "${FORWARDING_HEADERS_DIR}"
    "${GLIB_INCLUDE_DIRS}"
)

# 添加 GLIB 的链接函数库
set(helloworld_LIBRARIES
    HelloWorld::HelloWorld
    "${GLIB_LIBRARIES}"
)
```

## 自定义功能测试

在一些中大型项目中，开发者会通过自定义功能测试宏来管理是否开启某些功能特性。比如在一个项目的维护过程中，可能会增加一个新的功能，但这个功能可能会打来一些兼容性问题。此时，项目管理者会考虑使用自定义的功能测试宏：对已有的用户，关闭该功能，而对新的用户，则打开该功能。

本章介绍的 CMake 构建系统模板，为这一需求提供了良好的支持。

首先，我们在 `source/cmake/GlobalFeatures.cmake` 中定义功能开关：

```cmake
HELLOWORLD_OPTION_DEFINE(ENABLE_HTML "Toggle HTML parser" PUBLIC ON)
```

上面语句，使用 `HELLOWORLD_OPTION_DEFINE` 宏定义了一个名为 `ENABLE_HTML` 的变量，用于是否支持 HTML 的开关，默认打开。为了和用于检测系统基础库、第三方软件包的变量相区别，通常使用 `ENABLE_` 作为自定义功能测试变量的前缀。

在构建的配置阶段，我们可以通过 `cmake` 命令行参数覆盖自定义功能的默认设置。比如：

```console
$ cmake .. -DENABLE_HTML=OFF
```

在 `source/cmake/Options<PortName>.cmake` 文件中，也可以针对特定的平台覆盖这一默认设置。比如根据一些第三方软件包的查找结果关闭这一功能：

```cmake
if (not HAVE_GLIB)
    HELLOWORLD_OPTION_DEFAULT_PORT_VALUE(ENABLE_HTML PUBLIC OFF)
endif ()
```

上面的语句，在未检测到 Glib 时，关闭对 HTML 的支持。

最终，我们可在其他 CMake 描述文件中使用 `ENABLE_HTML` 变量，或者在 `cmakeconfig.h` 中看到 `ENABLE_HTML` 宏的定义。

