# -----------------------------------------------------------------------------
# Add components directories
# -----------------------------------------------------------------------------
add_subdirectory(wtf)

add_subdirectory(foobar)

if (ENABLE_API_TESTS)
    add_subdirectory(third-party/gtest)
#    add_subdirectory(test)
endif ()

#add_subdirectory(samples)

FOOBAR_INCLUDE_CONFIG_FILES_IF_EXISTS()
