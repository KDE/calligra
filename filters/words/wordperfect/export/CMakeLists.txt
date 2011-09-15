




########### next target ###############

set(wpexport_PART_SRCS wpexport.cc wp5.cc wp6.cc )


kde4_add_plugin(wpexport ${wpexport_PART_SRCS})



target_link_libraries(wpexport wordsexportfilters)

install(TARGETS wpexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_wp_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


