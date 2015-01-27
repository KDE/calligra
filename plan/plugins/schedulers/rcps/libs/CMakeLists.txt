if (NOT MSVC)
    add_definitions(-Wno-undef -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-parameter -Wno-discarded-qualifiers)
endif (NOT MSVC)

add_subdirectory(src)
