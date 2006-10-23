
set(koconverter_SRCS koconverter.cpp )

kde4_automoc(${koconverter_SRCS})

kde4_add_executable(koconverter ${koconverter_SRCS})

target_link_libraries(koconverter  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS koconverter  DESTINATION ${BIN_INSTALL_DIR})

