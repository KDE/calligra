

include_directories( ${KOFFICECORE_INCLUDES} )


########### next target ###############

set(kounavailpart_PART_SRCS kounavail.cc )

kde4_automoc(${kounavailpart_PART_SRCS})

kde4_add_plugin(kounavailpart WITH_PREFIX ${kounavailpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kounavailpart )

target_link_libraries(kounavailpart  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kounavailpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kounavail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

