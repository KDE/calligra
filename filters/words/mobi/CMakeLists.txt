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


add_library(calligra_filter_odt2mobi MODULE ${odt2mobi_PART_SRCS})
target_link_libraries(calligra_filter_odt2mobi komain Qt5::Svg KF5::KDELibs4Support)

install(TARGETS calligra_filter_odt2mobi  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
kcoreaddons_desktop_to_json(calligra_filter_odt2mobi calligra_filter_odt2mobi.desktop)


 ########### install files ###############

install( FILES  calligra_filter_odt2mobi.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
