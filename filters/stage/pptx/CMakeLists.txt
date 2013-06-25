add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30528)

include_directories(
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx
    ${KOMAIN_INCLUDES}
)

########### next target ###############

set(pptx2odp_PART_SRCS
    PptxImport.cpp
    PptxXmlDocumentReader.cpp
    PptxXmlSlideReader.cpp
    PptxXmlCommentsReader.cpp
    PptxXmlCommentAuthorsReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/ChartExport.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/NumberFormatParser.cpp
)

kde4_add_plugin(calligra_filter_pptx2odp ${pptx2odp_PART_SRCS})

target_link_libraries(
    calligra_filter_pptx2odp
    msooxml
    koodf2
    koodf
    komain
)

install(TARGETS calligra_filter_pptx2odp DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES
  kpresenter_pptx_import.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES stage_pptx_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
