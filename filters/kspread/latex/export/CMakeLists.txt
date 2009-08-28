add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOMAIN_INCLUDES})

########### next target ###############

set(kspreadlatexexport_PART_SRCS 
   cell.cc 
   column.cc 
   config.cc 
   document.cc 
   fileheader.cc 
   format.cc 
   kspreadlatexexportdiaImpl.cc 
   latexexport.cc 
   latexexportAdaptor.cc 
   map.cc 
   pen.cc 
   row.cc 
   spreadsheet.cc 
   table.cc 
   xmlparser.cc )

kde4_add_ui3_files(kspreadlatexexport_PART_SRCS latexexportdia.ui )

kde4_add_plugin(kspreadlatexexport WITH_PREFIX ${kspreadlatexexport_PART_SRCS})



target_link_libraries(kspreadlatexexport komain ${KDE4_KDE3SUPPORT_LIBS})

install(TARGETS kspreadlatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



