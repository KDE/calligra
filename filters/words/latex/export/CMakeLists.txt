

include_directories(${KFORMULA_INCLUDES} ${CMAKE_BINARY_DIR}/filters/)

########### next target ###############

set(wordslatexexport_PART_SRCS 
   anchor.cc 
   config.cc 
   document.cc 
   element.cc 
   fileheader.cc 
   footnote.cc 
   format.cc 
   formula.cc 
   key.cc 
   wordslatexexportdia.cc 
   latexexport.cc 
   latexexportAdaptor.cc 
   layout.cc 
   listtable.cc 
   para.cc 
   pixmapFrame.cc 
   table.cc 
   textFrame.cc 
   textformat.cc 
   textzone.cc 
   variableformat.cc 
   variablezone.cc 
   xml2latexparser.cc 
   xmlparser.cc )

kde4_add_ui3_files(wordslatexexport_PART_SRCS latexexportdia.ui )


kde4_add_plugin(wordslatexexport ${wordslatexexport_PART_SRCS})



target_link_libraries(wordslatexexport komain kformulalib ${IMAGEMAGICK_LIBRARIES} )

install(TARGETS wordslatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  words_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



