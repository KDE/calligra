




########### next target ###############

set(wmlimport_PART_SRCS wmlimport.cc wmlparser.cpp )

kde4_automoc(${wmlimport_PART_SRCS})

kde4_add_plugin(wmlimport WITH_PREFIX ${wmlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmlimport )

target_link_libraries(wmlimport  ${KDE4_KDECORE_LIBS} kofficecore kwordexportfilters)

install(TARGETS wmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(wmlexport_PART_SRCS wmlexport.cc )

kde4_automoc(${wmlexport_PART_SRCS})

kde4_add_plugin(wmlexport WITH_PREFIX ${wmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmlexport )

target_link_libraries(wmlexport  ${KDE4_KDECORE_LIBS} kwordexportfilters kofficecore )

install(TARGETS wmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wml_import.desktop kword_wml_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


