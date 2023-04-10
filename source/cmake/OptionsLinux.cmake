include(GNUInstallDirs)

FOOBAR_OPTION_BEGIN()

CALCULATE_LIBRARY_VERSIONS_FROM_LIBTOOL_TRIPLE(FOOBAR 0 0 0)

# These are shared variables, but we special case their definition so that we can use the
# CMAKE_INSTALL_* variables that are populated by the GNUInstallDirs macro.
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE PATH "Absolute path to library installation directory")
set(EXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_BINDIR}" CACHE PATH "Absolute path to executable installation directory")
set(LIBEXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/foobar" CACHE PATH "Absolute path to install executables executed by the library")
set(HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Absolute path to header installation directory")
set(FOOBAR_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/foobar" CACHE PATH "Absolute path to FooBar header installation directory")

add_definitions(-DBUILDING_LINUX__=1)
add_definitions(-DFOOBAR_API_VERSION_STRING="${FOOBAR_API_VERSION}")

find_package(GLIB 2.44.0 COMPONENTS gio gio-unix)
# find_package(HiBus 100)
find_package(LibXml2 2.8.0)
find_package(LibSoup 2.54.0)
find_package(CURL 7.60.0)
find_package(OpenSSL 1.1.1)
find_package(SQLite3 3.10.0)
find_package(MySQLClient 20.0.0)
find_package(ZLIB 1.2.0)

if (NOT GLIB_FOUND)
    set(ENABLE_SOCKET_STREAM_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_GLIB OFF)
else ()
    set(ENABLE_SOCKET_STREAM_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_GLIB ON)
endif ()

if (NOT LIBXML2_FOUND)
    set(ENABLE_XML_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_LIBXML2 OFF)
else ()
    set(ENABLE_XML_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_LIBXML2 ON)
endif ()

if (NOT LIBSOUP_FOUND)
    set(ENABLE_HTTP_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_LIBSOUP OFF)
else ()
    set(ENABLE_HTTP_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_LIBSOUP ON)
endif ()

if (NOT CURL_FOUND)
    set(ENABLE_HTTP_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_CURL OFF)
else ()
    set(ENABLE_HTTP_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_CURL ON)
endif ()

if (NOT SQLITE3_FOUND)
    set(ENABLE_LSQL_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_SQLITE3 OFF)
else ()
    set(ENABLE_LSQL_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_SQLITE3 ON)
endif ()

if (NOT MYSQLCLIENT_FOUND)
    set(ENABLE_RSQL_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_MYSQLCLIENT OFF)
else ()
    set(ENABLE_RSQL_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_MYSQLCLIENT ON)
endif ()

if (NOT HIBUS_FOUND)
    set(ENABLE_HIBUS_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_HIBUS OFF)
else ()
    set(ENABLE_HIBUS_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_HIBUS ON)
endif ()

if (NOT OPENSSL_FOUND)
    set(ENABLE_SSL_DEFAULT OFF)
    SET_AND_EXPOSE_TO_BUILD(HAVE_OPENSSL OFF)
else ()
    set(ENABLE_SSL_DEFAULT ON)
    SET_AND_EXPOSE_TO_BUILD(HAVE_OPENSSL ON)
endif ()

# Public options specific to the HybridOS port. Do not add any options here unless
# there is a strong reason we should support changing the value of the option,
# and the option is not relevant to any other FooBar ports.
#FOOBAR_OPTION_DEFINE(USE_SYSTEMD "Whether to enable journald logging" PUBLIC ON)

# Private options specific to the HybridOS port. Changing these options is
# completely unsupported. They are intended for use only by FooBar developers.
#FOOBAR_OPTION_DEFINE(USE_ANGLE_WEBGL "Whether to use ANGLE as WebGL backend." PRIVATE OFF)
#FOOBAR_OPTION_DEPEND(ENABLE_WEBGL ENABLE_GRAPHICS_CONTEXT_GL)

FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_XML PUBLIC ${ENABLE_XML_DEFAULT})
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_HTTP PUBLIC ${ENABLE_HTTP_DEFAULT})
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_LSQL PUBLIC ${ENABLE_LSQL_DEFAULT})
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_RSQL PUBLIC ${ENABLE_RSQL_DEFAULT})
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_HIBUS PUBLIC ${ENABLE_HIBUS_DEFAULT})
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_SSL PUBLIC ${ENABLE_SSL_DEFAULT})

# Finalize the value for all options. Do not attempt to use an option before
# this point, and do not attempt to change any option after this point.
FOOBAR_OPTION_END()

if (USE_LIBSECRET)
    find_package(Libsecret)
    if (NOT LIBSECRET_FOUND)
        message(FATAL_ERROR "libsecret is needed for USE_LIBSECRET")
    endif ()
endif ()

set(FooBar_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/src/foobar/foobar.pc)

# CMake does not automatically add --whole-archive when building shared objects from
# a list of convenience libraries. This can lead to missing symbols in the final output.
# We add --whole-archive to all libraries manually to prevent the linker from trimming
# symbols that we actually need later. With ld64 on darwin, we use -all_load instead.
macro(ADD_WHOLE_ARCHIVE_TO_LIBRARIES _list_name)
    if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        list(APPEND ${_list_name} -Wl,-all_load)
    else ()
        set(_tmp)
        foreach (item IN LISTS ${_list_name})
            if ("${item}" STREQUAL "PRIVATE" OR "${item}" STREQUAL "PUBLIC")
                list(APPEND _tmp "${item}")
            else ()
                list(APPEND _tmp -Wl,--whole-archive "${item}" -Wl,--no-whole-archive)
            endif ()
        endforeach ()
        set(${_list_name} ${_tmp})
    endif ()
endmacro()

#include(BubblewrapSandboxChecks)
