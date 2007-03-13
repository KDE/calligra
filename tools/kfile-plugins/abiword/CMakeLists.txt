include_directories( ${KOFFICECORE_INCLUDES})

set(kfile_abiword_SRCS kfile_abiword.cpp )

kde4_automoc(${kfile_abiword_SRCS})

kde4_add_plugin(kfile_abiword ${kfile_abiword_SRCS})



target_link_libraries(kfile_abiword  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kfile_abiword  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kfile_abiword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
