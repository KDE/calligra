include_directories(${KOMAIN_INCLUDES})

set(calligraconverter_SRCS calligraconverter.cpp)

add_executable(calligraconverter ${calligraconverter_SRCS})

target_link_libraries(calligraconverter komain KF5::KDELibs4Support)

install(TARGETS calligraconverter ${INSTALL_TARGETS_DEFAULT_ARGS})

