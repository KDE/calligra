


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(amiproimport_PART_SRCS amiproimport.cc amiproparser.cpp )


kde4_add_plugin(amiproimport WITH_PREFIX ${amiproimport_PART_SRCS})



target_link_libraries(amiproimport kwordexportfilters)

install(TARGETS amiproimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(amiproexport_PART_SRCS amiproexport.cc )

kde4_add_plugin(amiproexport WITH_PREFIX ${amiproexport_PART_SRCS})



target_link_libraries(amiproexport kwordexportfilters)

install(TARGETS amiproexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_amipro_import.desktop kword_amipro_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



