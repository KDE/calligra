
set(CMAKE_THREAD_PREFER_PTHREAD 1)

macro_optional_find_package(Threads)

if(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT AND CMAKE_THREAD_LIBS_INIT)
    set(HAVE_PTHREADS 1)
endif(Threads_FOUND AND CMAKE_USE_PTHREADS_INIT AND CMAKE_THREAD_LIBS_INIT)

set(librcps_LIB_SRCS
    decode.c
    initial.c
    fitness.c
    librcps.c
    ops.c
    repair.c
    slist.c
)

if (CMAKE_C_COMPILER_ID STREQUAL "SunPro")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -xc99")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

if(WIN32)
    kde4_add_library(rcps_plan STATIC ${librcps_LIB_SRCS})
else(WIN32)
    kde4_add_library(rcps_plan SHARED ${librcps_LIB_SRCS})
endif(WIN32)

target_link_libraries(
    rcps_plan
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(rcps_plan PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS rcps_plan ${INSTALL_TARGETS_DEFAULT_ARGS})
