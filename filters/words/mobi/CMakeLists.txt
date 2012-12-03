include_directories( 
  ${CMAKE_SOURCE_DIR}/filters/words/epub
)
set(exportMobi_PART_SRCS
  exportmobi.cpp
  MobiHeaderGenerator.cpp
  MobiFile.cpp
  PalmDocCompression.cpp
  OdtMobiHtmlConverter.cpp
  
  ${CMAKE_SOURCE_DIR}/filters/words/epub/FileCollector.cpp
  ${CMAKE_SOURCE_DIR}/filters/words/epub/OdfParser.cpp
)


kde4_add_plugin(exportMobi ${exportMobi_PART_SRCS})


target_link_libraries(exportMobi komain)

install(TARGETS exportMobi  DESTINATION ${PLUGIN_INSTALL_DIR})



 ########### install files ###############

install( FILES  words_Mobi_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
