########### next target ###############

set(htmlexport_PART_SRCS 
   htmlexport.cc 
   ExportDialog.cc 
   ExportFilter.cc 
   ExportCss.cc 
   ExportDocStruct.cc 
   ExportBasic.cc )


kde4_add_ui3_files(htmlexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(htmlexport WITH_PREFIX ${htmlexport_PART_SRCS})

target_link_libraries(htmlexport kwordexportfilters ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS htmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

