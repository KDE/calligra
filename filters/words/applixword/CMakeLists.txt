########### next target ###############

set(applixword2odt_PART_SRCS applixwordimport.cc )


kde4_add_plugin(calligra_filter_applixword2odt ${applixword2odt_PART_SRCS})



target_link_libraries(calligra_filter_applixword2odt komain)

install(TARGETS calligra_filter_applixword2odt  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_applixword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


