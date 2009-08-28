
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(applixwordimport_PART_SRCS applixwordimport.cc )


kde4_add_plugin(applixwordimport WITH_PREFIX ${applixwordimport_PART_SRCS})



target_link_libraries(applixwordimport komain)

install(TARGETS applixwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_applixword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


