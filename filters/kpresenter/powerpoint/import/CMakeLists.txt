




########### next target ###############

set(powerpointimport_PART_SRCS powerpointimport.cc ${libppt_SRCS} )

kde4_automoc(${powerpointimport_PART_SRCS})

kde4_add_plugin(powerpointimport WITH_PREFIX ${powerpointimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} powerpointimport )

target_link_libraries(powerpointimport  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS powerpointimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_powerpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

