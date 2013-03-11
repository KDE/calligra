include_directories(${KOMAIN_INCLUDES})

########### next target ###############

set(kspreadlatexexport_PART_SRCS 
   cell.cc 
   column.cc 
   config.cc 
   document.cc 
   fileheader.cc 
   format.cc 
   latexexportdialog.cc
   latexexport.cc 
   latexexportAdaptor.cc 
   map.cc 
   pen.cc 
   row.cc 
   spreadsheet.cc 
   table.cc 
   xmlparser.cc )

kde4_add_ui_files(kspreadlatexexport_PART_SRCS latexexportwidget.ui )

kde4_add_plugin(kspreadlatexexport ${kspreadlatexexport_PART_SRCS})



target_link_libraries(kspreadlatexexport komain)

install(TARGETS kspreadlatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



