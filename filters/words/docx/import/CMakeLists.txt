add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30526)

include_directories(
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx # for Charting
    ${KOMAIN_INCLUDES}
)

########### next target ###############

set(docx2odt_PART_SRCS
    DocxXmlStylesReader.cpp
    DocxXmlFontTableReader.cpp
    DocxXmlCommentsReader.cpp
    DocxXmlDocumentReader.cpp
    DocxXmlHeaderReader.cpp
    DocxXmlFooterReader.cpp
    DocxXmlNumberingReader.cpp
    DocxXmlFootnoteReader.cpp
    DocxXmlEndnoteReader.cpp
    DocxXmlSettingsReader.cpp
    DocxImport.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/ChartExport.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/NumberFormatParser.cpp
)

kde4_add_plugin(calligra_filter_docx2odt ${docx2odt_PART_SRCS})

target_link_libraries(
    calligra_filter_docx2odt
    msooxml
    koodf2
    koodf
    komain
)

install(TARGETS calligra_filter_docx2odt DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

IF(FALSE) # not implemented yet

set(docxexport_PART_SRCS DocxExport.cpp ExportFilter.cpp ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_add_plugin(docxexport ${docxexport_PART_SRCS})



target_link_libraries(
    docxexport
    msooxml
    komain
    koodf
)

install(TARGETS docxexport DESTINATION ${PLUGIN_INSTALL_DIR})

ENDIF(FALSE)

########### next target ###############

########### install files ###############

install( FILES
  words_docx_import.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES words_docx_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
