set(oopptimport_PART_SRCS oopptimport.cc)

kde4_add_plugin(oopptimport ${oopptimport_PART_SRCS})

target_link_libraries(oopptimport komain)

install(TARGETS oopptimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kpresenter_ooppt_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS KOfficeOODocumentConverter.py DESTINATION ${BIN_INSTALL_DIR})
