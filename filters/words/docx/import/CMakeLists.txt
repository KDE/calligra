include_directories(
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/libmso # For NumberFormatParser
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx # for the XlsxChartReader
    ${KOMAIN_INCLUDES}
)

########### next target ###############

set(docx2odt_PART_SRCS
    DocxDebug.cpp
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
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
)

add_library(calligra_filter_docx2odt MODULE ${docx2odt_PART_SRCS})

target_link_libraries(
    calligra_filter_docx2odt
    komsooxml
    mso
    koodf2
    koodf
    komain
)

install(TARGETS calligra_filter_docx2odt DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)


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

install(TARGETS docxexport DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()

########### next target ###############

########### install files ###############

if(SHOULD_BUILD_OKULAR_GENERATOR_DOCX)
    kcoreaddons_add_plugin(okularGenerator_docx_calligra
        INSTALL_NAMESPACE "okular_generators"
        SOURCES OkularDocxGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_docx_calligra
        kookularGenerator_odt
    )

    install( PROGRAMS okularApplication_docx_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
