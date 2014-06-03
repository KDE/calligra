include_directories(${KOMAIN_INCLUDES})

########### next target ###############

set(kspread2tex_PART_SRCS
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

kde4_add_ui_files(kspread2tex_PART_SRCS latexexportwidget.ui )

kde4_add_plugin(calligra_filter_kspread2tex ${kspread2tex_PART_SRCS})



target_link_libraries(calligra_filter_kspread2tex komain)

install(TARGETS calligra_filter_kspread2tex  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_kspread2tex.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



