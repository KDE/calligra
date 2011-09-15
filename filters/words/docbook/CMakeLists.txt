


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(docbookexport_PART_SRCS docbookexport.cc )


kde4_add_plugin(docbookexport ${docbookexport_PART_SRCS})



target_link_libraries(docbookexport wordsexportfilters)

install(TARGETS docbookexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_docbook_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

