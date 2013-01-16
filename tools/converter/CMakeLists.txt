include_directories(${KOMAIN_INCLUDES})

set(calligraconverter_SRCS calligraconverter.cpp)

kde4_add_executable(calligraconverter ${calligraconverter_SRCS})

target_link_libraries(calligraconverter komain)

install(TARGETS calligraconverter ${INSTALL_TARGETS_DEFAULT_ARGS})

