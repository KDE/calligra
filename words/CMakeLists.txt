project(words)
include_directories(
        ${KOMAIN_INCLUDES}
)
if(KDE4_BUILD_TESTS)
    # only with this definition will the FOO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( part )
add_subdirectory( plugins )
add_subdirectory( styles )
