include_directories( ${KOMAIN_INCLUDES})

set(calligra_SRCS main.cpp )

kde4_add_executable(calligra ${calligra_SRCS})

target_link_libraries(calligra komain)

install(TARGETS calligra ${INSTALL_TARGETS_DEFAULT_ARGS})
