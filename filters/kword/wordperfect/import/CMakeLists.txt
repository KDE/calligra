

include_directories(${WPD_INCLUDE_DIR})


########### next target ###############

set(wpimport_PART_SRCS wpimport.cc )

kde4_automoc(${wpimport_PART_SRCS})

kde4_add_plugin(wpimport WITH_PREFIX ${wpimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wpimport )

target_link_libraries(wpimport  ${KDE4_KDECORE_LIBS} kofficecore ${WPD_LIBRARIES} )

install(TARGETS wpimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wp_import.desktop   DESTINATION ${SERVICES_INSTALL_DIR})



