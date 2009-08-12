include_directories( ${KOMAIN_INCLUDES})

set(filtertester_SRCS filtertester.cpp )

kde4_add_executable(filtertester ${filtertester_SRCS})

target_link_libraries(filtertester komain ${KDE4_KIO_LIBS})

install(TARGETS filtertester  ${INSTALL_TARGETS_DEFAULT_ARGS})

