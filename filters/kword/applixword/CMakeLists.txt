




########### next target ###############

set(applixwordimport_PART_SRCS applixwordimport.cc )

kde4_automoc(${applixwordimport_PART_SRCS})

kde4_add_plugin(applixwordimport WITH_PREFIX ${applixwordimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} applixwordimport )

target_link_libraries(applixwordimport  ${KDE4_KDECORE_LIBS} kofficecore kstore )

install(TARGETS applixwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_applixword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


