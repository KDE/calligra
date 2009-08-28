


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(palmdocimport_PART_SRCS palmdocimport.cc palmdb.cpp palmdoc.cpp )


kde4_add_plugin(palmdocimport WITH_PREFIX ${palmdocimport_PART_SRCS})



target_link_libraries(palmdocimport komain)

install(TARGETS palmdocimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(palmdocexport_PART_SRCS palmdocexport.cc palmdb.cpp palmdoc.cpp )

kde4_add_plugin(palmdocexport WITH_PREFIX ${palmdocexport_PART_SRCS})



target_link_libraries(palmdocexport kwordexportfilters)

install(TARGETS palmdocexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_palmdoc_import.desktop kword_palmdoc_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



