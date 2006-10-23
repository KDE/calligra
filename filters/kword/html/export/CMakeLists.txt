




########### next target ###############

set(htmlexport_PART_SRCS 
   htmlexport.cc 
   ExportDialog.cc 
   ExportFilter.cc 
   ExportCss.cc 
   ExportDocStruct.cc 
   ExportBasic.cc )

kde4_automoc(${htmlexport_PART_SRCS})

kde4_add_ui3_files(htmlexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(htmlexport WITH_PREFIX ${htmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} htmlexport )

target_link_libraries(htmlexport  ${KDE4_KDECORE_LIBS} kwordexportfilters ${KDE4_KDEUI_LIBS})

install(TARGETS htmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

