




########### next target ###############

set(rtfexport_PART_SRCS rtfexport.cc ExportFilter.cc )


kde4_add_plugin(rtfexport ${rtfexport_PART_SRCS})



target_link_libraries(rtfexport wordsexportfilters)

install(TARGETS rtfexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_rtf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

