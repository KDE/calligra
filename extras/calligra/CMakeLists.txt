include_directories( ${KOMAIN_INCLUDES})

set(calligra_SRCS main.cpp )

add_executable(calligra ${calligra_SRCS})

target_link_libraries(calligra komain ${KDE4_KIO_LIBS})

install(TARGETS calligra ${INSTALL_TARGETS_DEFAULT_ARGS})
