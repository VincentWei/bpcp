set(WTF_PLATFORM_WIN_CAIRO 1)

include(OptionsWin)

find_package(LibXml2 2.9.7 REQUIRED)
find_package(OpenSSL 2.0.0 REQUIRED)
find_package(SQLite3 3.23.1 REQUIRED)
find_package(ZLIB 1.2.11 REQUIRED)

# TODO: Add a check for HAVE_RSA_PSS for support of CryptoAlgorithmRSA_PSS
# https://bugs.webkit.org/show_bug.cgi?id=206635

SET_AND_EXPOSE_TO_BUILD(USE_CURL ON)
SET_AND_EXPOSE_TO_BUILD(USE_OPENSSL ON)
SET_AND_EXPOSE_TO_BUILD(ENABLE_DEVELOPER_MODE ${DEVELOPER_MODE})

set(COREFOUNDATION_LIBRARY CFlite)

add_definitions(-DWTF_PLATFORM_WIN_CAIRO=1)
add_definitions(-DNOCRYPT)

# Override headers directories
set(WTF_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WTF/Headers)
set(FooBar_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/FooBar/Headers)
set(FooBar_PRIVATE_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/FooBar/PrivateHeaders)

# Override derived sources directories
set(WTF_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/WTF/DerivedSources)
set(FooBar_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/FooBar/DerivedSources)

# Override library types
set(FooBar_LIBRARY_TYPE OBJECT)
set(FooBarTestSupport_LIBRARY_TYPE OBJECT)
