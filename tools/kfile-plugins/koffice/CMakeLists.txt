
set(kfile_koffice_PART_SRCS kfile_koffice.cpp )

kde4_automoc(${kfile_koffice_PART_SRCS})

kde4_add_plugin(kfile_koffice ${kfile_koffice_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_koffice )

target_link_libraries(kfile_koffice  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kfile_koffice  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kfile_koffice.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
