
set(librcps_LIB_SRCS
    decode.c
    initial.c
    fitness.c
    librcps.c
    ops.c
    repair.c
    slist.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

kde4_add_library(librcps_plan SHARED ${librcps_LIB_SRCS})

target_link_libraries(
    librcps_plan
    pthread
)

install(TARGETS librcps_plan ${INSTALL_TARGETS_DEFAULT_ARGS})
