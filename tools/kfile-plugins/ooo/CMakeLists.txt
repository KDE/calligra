
set(kfile_ooo_PART_SRCS kfile_ooo.cpp )

kde4_automoc(${kfile_ooo_PART_SRCS})

kde4_add_plugin(kfile_ooo ${kfile_ooo_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_ooo )

target_link_libraries(kfile_ooo  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kfile_ooo  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kfile_ooo.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

