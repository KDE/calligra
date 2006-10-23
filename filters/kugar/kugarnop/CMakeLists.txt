




########### next target ###############

set(kugarnopimport_PART_SRCS kugarnopimport.cpp )

kde4_automoc(${kugarnopimport_PART_SRCS})

kde4_add_plugin(kugarnopimport WITH_PREFIX ${kugarnopimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kugarnopimport )

target_link_libraries(kugarnopimport  ${KDE4_KDECORE_LIBS} )

install(TARGETS kugarnopimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kugar_kugar_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





