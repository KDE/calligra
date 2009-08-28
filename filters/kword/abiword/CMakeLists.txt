

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(abiwordimport_PART_SRCS 
   abiwordimport.cc 
   ImportHelpers.cc 
   ImportFormatting.cc 
   ImportStyle.cc 
   ImportField.cc )


kde4_add_plugin(abiwordimport WITH_PREFIX ${abiwordimport_PART_SRCS})



target_link_libraries(abiwordimport kwordexportfilters)

install(TARGETS abiwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(abiwordexport_PART_SRCS abiwordexport.cc )

kde4_add_plugin(abiwordexport WITH_PREFIX ${abiwordexport_PART_SRCS})



target_link_libraries( abiwordexport kwordexportfilters)

install(TARGETS abiwordexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_abiword_import.desktop kword_abiword_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


