include_directories(${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${CMAKE_SOURCE_DIR}/tables)

set(cstester_SRCS cstester.cpp )

kde4_add_executable(cstester ${cstester_SRCS})

target_link_libraries(cstester komain kopageapp calligratablescommon)

install(TARGETS cstester  ${INSTALL_TARGETS_DEFAULT_ARGS})

