set(FooBar_OUTPUT_NAME foobar)

list(APPEND FooBar_PRIVATE_INCLUDE_DIRECTORIES
)

list(APPEND FooBar_UNIFIED_SOURCE_LIST_FILES
)

list(APPEND FooBar_SOURCES
)

list(APPEND FooBar_LIBRARIES
    -lpthread
)

if (ENABLE_SOCKET_STREAM)
    list(APPEND FooBar_SYSTEM_INCLUDE_DIRECTORIES
        ${GLIB_INCLUDE_DIRS}
    )

    list(APPEND FooBar_LIBRARIES
        ${GLIB_GIO_LIBRARIES}
        ${GLIB_LIBRARIES}
    )
endif ()

configure_file(ports/linux/foobar.pc.in ${FooBar_PKGCONFIG_FILE} @ONLY)
install(FILES "${FooBar_PKGCONFIG_FILE}"
        DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)

