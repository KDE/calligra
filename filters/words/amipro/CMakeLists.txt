


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(amiproimport_PART_SRCS amiproimport.cc amiproparser.cpp )


kde4_add_plugin(amiproimport ${amiproimport_PART_SRCS})



target_link_libraries(amiproimport wordsexportfilters)

install(TARGETS amiproimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(amiproexport_PART_SRCS amiproexport.cc )

kde4_add_plugin(amiproexport ${amiproexport_PART_SRCS})



target_link_libraries(amiproexport wordsexportfilters)

install(TARGETS amiproexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_amipro_import.desktop words_amipro_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



