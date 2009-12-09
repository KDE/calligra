########### next target ###############

set(asciiimport_PART_SRCS asciiimport.cc ImportDialog.cc )


kde4_add_ui_files(asciiimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(asciiimport WITH_PREFIX ${asciiimport_PART_SRCS})

target_link_libraries(asciiimport kwordexportfilters)

install(TARGETS asciiimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(asciiexport_PART_SRCS asciiexport.cc ExportDialog.cc )

kde4_add_ui_files(asciiexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(asciiexport WITH_PREFIX ${asciiexport_PART_SRCS})



target_link_libraries(asciiexport kwordexportfilters)

install(TARGETS asciiexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_ascii_import.desktop kword_ascii_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



