include_directories( ${KOFFICECORE_INCLUDES})

set(kfile_gnumeric_SRCS kfile_gnumeric.cpp )

kde4_automoc(${kfile_gnumeric_SRCS})

kde4_add_plugin(kfile_gnumeric ${kfile_gnumeric_SRCS})



target_link_libraries(kfile_gnumeric  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kfile_gnumeric  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kfile_gnumeric.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

