include_directories(
    ${KOMAIN_INCLUDES}
    ${KOODF2_INCLUDES} # For charts
    ${CMAKE_CURRENT_SOURCE_DIRECTORY}
    ${CMAKE_SOURCE_DIR}/filters/libmso # For NumberFormatParser
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx # For ChartExport  FIXME: Remove when moved to libodf2
)

########### next target ###############

set(msooxml_LIB_SRCS
    MsooXmlDebug.cpp
    MsooXmlGlobal.cpp
    MsooXmlUtils.cpp
    MsooXmlReader.cpp
    MsooXmlCommonReader.cpp
    MsooXmlThemesReader.cpp
    MsooXmlRelationshipsReader.cpp
    MsooXmlRelationships.cpp
    MsooXmlImport.cpp
    MsooXmlDocPropertiesReader.cpp
    MsooXmlDiagramReader.cpp
    MsooXmlDiagramReader_p.cpp
    ooxml_pole.cpp
    MsooXmlDrawingTableStyleReader.cpp
    MsooXmlDrawingTableStyle.cpp
    MsooXmlTableStyle.cpp
    VmlDrawingReader.cpp
    PredefinedShapeHelper.cpp
    ComplexShapeHandler.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxChartOdfWriter.cpp
    # TODO: move back to kolibs
    KoOdfExporter.cpp
)

add_library(komsooxml SHARED ${msooxml_LIB_SRCS})
generate_export_header(komsooxml BASE_NAME komsooxml)

target_link_libraries(komsooxml
    PUBLIC
        mso
        koodf2
        komain
    PRIVATE
        KF6::Archive
    )

set_target_properties(komsooxml PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
install(TARGETS komsooxml ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (FALSE) # these headers are private for now
install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/komsooxml_export.h
    MsooXmlContentTypes.h
    MsooXmlReader.h
    MsooXmlReader_p.h
    MsooXmlCommonReaderImpl.h
    MsooXmlCommonReader.h
    MsooXmlCommonReaderMethods.h
    MsooXmlSchemas.h
    MsooXmlThemesReader.h
    MsooXmlTheme.h
    MsooXmlUtils.h
    MsooXmlRelationships.h
    MsooXmlImport.h
    MsooXmlCommentsReader.h
    MsooXmlDocPropertiesReader.h
    MsooXmlNotesReader.h
    MsooXmlDiagramReader.h
    MsooXmlDiagramReader_p.h
    MsooXmlDrawingTableStyleReader.h
    MsooXmlDrawingTableStyle.h
    MsooXmlDrawingMLShared.h
    MsooXmlDrawingMLSharedImpl.h
    VmlDrawingReader.h
    ComplexShapeHandler.h
    PredefinedShapeHelper.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/msooxml COMPONENT Devel
)
endif ()
