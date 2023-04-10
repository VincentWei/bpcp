# FIXME: These should line up with versions in Version.xcconfig
set(FOOBAR_MAC_VERSION 0.0.1)
set(MACOSX_FRAMEWORK_BUNDLE_VERSION 0.0.1)

find_package(LibXml2 2.8.0)
find_package(LibXslt 1.1.7)
find_package(CURL 7.60.0)
find_package(OpenSSL 1.1.1)
find_package(SQLite3 3.10.0)

FOOBAR_OPTION_BEGIN()
# Private options shared with other FooBar ports. Add options here only if
# we need a value different from the default defined in GlobalFeatures.cmake.

FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_XML PUBLIC OFF)
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_HTTP PUBLIC OFF)
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_LSQL PUBLIC OFF)
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_RSQL PUBLIC OFF)
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_HIBUS PUBLIC OFF)
FOOBAR_OPTION_DEFAULT_PORT_VALUE(ENABLE_SSL PUBLIC OFF)

FOOBAR_OPTION_END()

set(FooBar_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/src/foobar/foobar.pc)

set(FooBar_LIBRARY_TYPE SHARED)
set(FooBarTestSupport_LIBRARY_TYPE SHARED)

