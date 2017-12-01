if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-error)
endif ()

set(CMAKE_THREAD_PREFER_PTHREAD 1)

macro_optional_find_package(Threads)

if(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT AND CMAKE_THREAD_LIBS_INIT)
    set(HAVE_PTHREADS 1)
endif()

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

if(WIN32)
    add_library(rcps_plan STATIC ${librcps_LIB_SRCS})
else()
    add_library(rcps_plan SHARED ${librcps_LIB_SRCS})
endif()

target_link_libraries(
    rcps_plan
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(rcps_plan PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS rcps_plan ${INSTALL_TARGETS_DEFAULT_ARGS})
