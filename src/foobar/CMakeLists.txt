set_property(DIRECTORY . PROPERTY FOLDER "FooBar")

include(GlobalCommon)
include(MyMacros.cmake)

configure_file(include/foobar_version.h.in ${FooBar_DERIVED_SOURCES_DIR}/foobar_version.h)
configure_file(include/foobar_features.h.in ${FooBar_DERIVED_SOURCES_DIR}/foobar_features.h)

set(FooBar_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${FOOBAR_DIR}"
    "${FOOBAR_DIR}/include"
    "${FooBar_DERIVED_SOURCES_DIR}"
    "${FORWARDING_HEADERS_DIR}"
    "${CMAKE_BINARY_DIR}/src/foobar"
)

# TODO: List the headers should be installed to system here.
set(FooBar_INSTALLED_HEADERS
    "${FooBar_DERIVED_SOURCES_DIR}/foobar_version.h"
    "${FooBar_DERIVED_SOURCES_DIR}/foobar_features.h"
    "${FOOBAR_DIR}/include/foobar_macros.h"
    "${FOOBAR_DIR}/include/foobar_errors.h"
    "${FOOBAR_DIR}/include/foobar_utils.h"
    "${FOOBAR_DIR}/include/foobar_ports.h"
    "${FOOBAR_DIR}/include/foobar.h"
)

# TODO: List the directorires which do not contain any platform-dependent C source files
set(FooBar_PLATFORM_INDEPENDENT_DIRS
    "instance"
    "utils"
    "foo"
    "bar"
)

# This macro will append all C/CXX source files in ${FooBar_PLATFORM_INDEPENDENT_DIRS}
# to ${FooBar_SOURCES}
APPEND_ALL_SOURCE_FILES_IN_DIRLIST(FooBar_SOURCES "${FooBar_PLATFORM_INDEPENDENT_DIRS}")

# TODO: List the source files individually.
list(APPEND FooBar_SOURCES
    "${FOOBAR_DIR}/ports/vasprintf.c"
#    "${FOOBAR_DIR}/warnings/bin2c.c"
#    "${FOOBAR_DIR}/warnings/unused.c"
#    "${FOOBAR_DIR}/warnings/preprocessor.c"
#    "${FOOBAR_DIR}/warnings/uninitialized.c"
#    "${FOOBAR_DIR}/warnings/typesafe.c"
    "${FOOBAR_DIR}/warnings/format.c"
    "${FOOBAR_DIR}/warnings/bugs-not-warned.c"
)

# Put the CXX source files which can be unified in `Sources.txt`
list(APPEND FooBar_UNIFIED_SOURCE_LIST_FILES
    "Sources.txt"
)

set(FooBar_LIBRARIES)

# TODO: the source files and libraries for optional features.
if (ENABLE_LSQL)
    list(APPEND FooBar_LIBRARIES SQLite::SQLite3)
endif ()

if (ENABLE_XML)
    list(APPEND FooBar_LIBRARIES LibXml2::LibXml2)
endif ()

set(FooBar_INTERFACE_LIBRARIES FooBar)
set(FooBar_INTERFACE_INCLUDE_DIRECTORIES ${FooBar_PRIVATE_FRAMEWORK_HEADERS_DIR})

FOOBAR_FRAMEWORK_DECLARE(FooBar)
FOOBAR_INCLUDE_CONFIG_FILES_IF_EXISTS()

FOOBAR_WRAP_SOURCELIST(${FooBar_SOURCES})

FOOBAR_COPY_FILES(FooBar_CopyPrivateHeaders
    DESTINATION ${FooBar_PRIVATE_FRAMEWORK_HEADERS_DIR}/foobar
    FILES ${FooBar_PRIVATE_FRAMEWORK_HEADERS}
    FLATTENED
)
list(APPEND FooBar_INTERFACE_DEPENDENCIES FooBar_CopyPrivateHeaders)
# FooBar_CopyPrivateHeaders needs to have a direct or indirect
# dependency of FooBar target for CMake Visual Studio generator to
# eliminate duplicated custom commands. Otherwise, CSSPropertyNames.h
# will be generated in both projects.
if (NOT INTERNAL_BUILD)
    add_dependencies(FooBar_CopyPrivateHeaders FooBar)
endif ()

FOOBAR_COMPUTE_SOURCES(FooBar)
FOOBAR_FRAMEWORK(FooBar)
FOOBAR_FRAMEWORK_TARGET(FooBar)

# The -ftree-sra optimization (implicit with -O2) causes crashes when
# allocating large chunks of memory using bmalloc on Intel 32bit.
# See https://bugs.webkit.org/show_bug.cgi?id=146440
string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} LOWERCASE_CMAKE_HOST_SYSTEM_PROCESSOR)
if (CMAKE_COMPILER_IS_GNUCXX AND "${LOWERCASE_CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "(i[3-6]86|x86)$")
    FOOBAR_ADD_TARGET_CXX_FLAGS(FooBar -fno-tree-sra)
endif ()

if (${FooBar_LIBRARY_TYPE} MATCHES "SHARED")
    set_target_properties(FooBar PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
    install(TARGETS FooBar DESTINATION "${LIB_INSTALL_DIR}")
endif ()

if (MSVC)
    set_target_properties(FooBar PROPERTIES
        COMPILE_PDB_NAME ${FooBar_OUTPUT_NAME}
    )
endif ()

# add_subdirectory(res)

install(TARGETS FooBar
        DESTINATION "${LIB_INSTALL_DIR}/"
)

install(FILES ${FooBar_INSTALLED_HEADERS}
        DESTINATION "${HEADER_INSTALL_DIR}/foobar"
)
