include_directories(
  ${CMAKE_SOURCE_DIR}/filters/words/epub
)
set(odt2mobi_PART_SRCS
  exportmobi.cpp
  MobiHeaderGenerator.cpp
  MobiFile.cpp
  PalmDocCompression.cpp
  OdtMobiHtmlConverter.cpp

  ${CMAKE_SOURCE_DIR}/filters/words/epub/FileCollector.cpp
  ${CMAKE_SOURCE_DIR}/filters/words/epub/OdfParser.cpp
)


kde4_add_plugin(calligra_filter_odt2mobi ${odt2mobi_PART_SRCS})


target_link_libraries(calligra_filter_odt2mobi komain)

install(TARGETS calligra_filter_odt2mobi  DESTINATION ${PLUGIN_INSTALL_DIR})



 ########### install files ###############

install( FILES  words_Mobi_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
