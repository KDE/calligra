

include_directories( ${KOMAIN_INCLUDES} )


########### next target ###############

set(kounavailpart_PART_SRCS kounavail.cc )


kde4_add_plugin(kounavailpart ${kounavailpart_PART_SRCS})



target_link_libraries(kounavailpart ${KOMAIN_LIBRARY} )

install(TARGETS kounavailpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kounavail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

