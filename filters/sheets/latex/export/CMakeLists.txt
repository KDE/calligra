include_directories(${KOMAIN_INCLUDES})

########### next target ###############

set(kspread2tex_PART_SRCS
   LatexDebug.cpp
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

target_link_libraries(calligra_filter_kspread2tex komain)

install(TARGETS calligra_filter_kspread2tex  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
