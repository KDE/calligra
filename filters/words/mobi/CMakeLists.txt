include_directories(
  ${CMAKE_SOURCE_DIR}/filters/words/epub
)
set(odt2mobi_PART_SRCS
  MobiExportDebug.cpp
  exportmobi.cpp
  MobiHeaderGenerator.cpp
  MobiFile.cpp
  PalmDocCompression.cpp
  OdtMobiHtmlConverter.cpp

  ${CMAKE_SOURCE_DIR}/filters/words/epub/FileCollector.cpp
  ${CMAKE_SOURCE_DIR}/filters/words/epub/OdfParser.cpp
)


add_library(calligra_filter_odt2mobi MODULE ${odt2mobi_PART_SRCS})
set_target_properties(calligra_filter_odt2mobi PROPERTIES COMPILE_DEFINITIONS "DEBUG_MOBI=1")

target_link_libraries(calligra_filter_odt2mobi komain Qt5::Svg)

install(TARGETS calligra_filter_odt2mobi  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_odt2mobi calligra_filter_odt2mobi.desktop)


 ########### install files ###############

install( FILES  calligra_filter_odt2mobi.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
