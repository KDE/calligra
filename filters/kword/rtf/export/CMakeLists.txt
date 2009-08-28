




########### next target ###############

set(rtfexport_PART_SRCS rtfexport.cc ExportFilter.cc )


kde4_add_plugin(rtfexport WITH_PREFIX ${rtfexport_PART_SRCS})



target_link_libraries(rtfexport kwordexportfilters)

install(TARGETS rtfexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_rtf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

