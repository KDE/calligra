add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30528)

include_directories( ${CMAKE_SOURCE_DIR}/filters/libmsooxml
                     ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx
)

########### next target ###############

set(pptximport_PART_SRCS
    PptxImport.cpp
    PptxXmlDocumentReader.cpp
    PptxXmlSlideReader.cpp
    PptxXmlCommentsReader.cpp
    PptxXmlCommentAuthorsReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/ChartExport.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/NumberFormatParser.cpp
)

kde4_add_plugin(pptximport ${pptximport_PART_SRCS})

target_link_libraries(
    pptximport
    msooxml
    koodf
    komain
)

install(TARGETS pptximport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES
  kpresenter_pptx_import.desktop
#  kpresenter_pptx_export.desktop
  stage_pptx_thumbnail.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)
