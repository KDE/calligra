if (NOT MSVC)
    add_definitions(-Wno-undef -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-parameter)
endif (NOT MSVC)

add_subdirectory(src)
