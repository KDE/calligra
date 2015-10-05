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

ki18n_wrap_ui(kspread2tex_PART_SRCS latexexportwidget.ui )

add_library(calligra_filter_kspread2tex MODULE ${kspread2tex_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_kspread2tex calligra_filter_kspread2tex.desktop)

target_link_libraries(calligra_filter_kspread2tex komain KF5::KDELibs4Support)

install(TARGETS calligra_filter_kspread2tex  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_kspread2tex.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)



