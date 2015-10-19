add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30526)

include_directories(
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/libmso # For NumberFormatParser
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx # for the XlsxChartReader
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
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxChartOdfWriter.cpp
    ${CMAKE_SOURCE_DIR}/filters/libodf2/chart/KoOdfChartWriter.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
)

add_library(calligra_filter_docx2odt MODULE ${docx2odt_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_docx2odt calligra_filter_docx2odt.desktop)

target_link_libraries(
    calligra_filter_docx2odt
    komsooxml
    mso
    koodf2
    koodf
    komain
    KF5::KDELibs4Support
)

install(TARGETS calligra_filter_docx2odt DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### next target ###############

if(FALSE) # not implemented yet

set(docxexport_PART_SRCS DocxExport.cpp ExportFilter.cpp ${libcommon_SRCS} )

add_library(docxexport MODULE ${docxexport_PART_SRCS})



target_link_libraries(
    docxexport
    komsooxml
    komain
    koodf
)

install(TARGETS docxexport DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

endif()

########### next target ###############

########### install files ###############

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES words_docx_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_DOCX)
    install( FILES libokularGenerator_docx_calligra.desktop okularDocx_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_docx_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
