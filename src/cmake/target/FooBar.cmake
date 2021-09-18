if (NOT TARGET FooBar::FooBar)
    if (NOT INTERNAL_BUILD)
        message(FATAL_ERROR "FooBar::FooBar target not found")
    endif ()

    # This should be moved to an if block if the Apple Mac/iOS build moves completely to CMake
    # Just assuming Windows for the moment
    add_library(FooBar::FooBar STATIC IMPORTED)
    set_target_properties(FooBar::FooBar PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIBRARIES_LINK_DIR}/FooBar${DEBUG_SUFFIX}.lib
    )
    set(FooBar_PRIVATE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/../include/private")
    target_include_directories(FooBar::FooBar INTERFACE
        ${FooBar_PRIVATE_FRAMEWORK_HEADERS_DIR}
    )
endif ()
