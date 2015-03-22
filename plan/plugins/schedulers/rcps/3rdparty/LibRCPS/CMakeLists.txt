if (NOT MSVC)
    add_definitions(-Wno-undef -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-parameter -Wno-discarded-qualifiers)
endif ()

add_subdirectory(src)
