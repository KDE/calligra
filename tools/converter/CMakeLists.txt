include_directories( ${KOMAIN_INCLUDES})

set(koconverter_SRCS koconverter.cpp )

kde4_add_executable(koconverter ${koconverter_SRCS})

target_link_libraries(koconverter komain ${KDE4_KIO_LIBS})

install(TARGETS koconverter  ${INSTALL_TARGETS_DEFAULT_ARGS})

