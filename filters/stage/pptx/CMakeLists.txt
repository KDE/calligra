include_directories(
    ${KOODF2_INCLUDES}
    ${CMAKE_SOURCE_DIR}/filters/libmsooxml
    ${CMAKE_SOURCE_DIR}/filters/libmso
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx
    ${KOMAIN_INCLUDES}
)

########### next target ###############

set(pptx2odp_PART_SRCS
    PptxDebug.cpp
    PptxImport.cpp
    PptxXmlDocumentReader.cpp
    PptxXmlSlideReader.cpp
    PptxXmlCommentsReader.cpp
    PptxXmlCommentAuthorsReader.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxChartOdfWriter.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
)

add_library(calligra_filter_pptx2odp MODULE ${pptx2odp_PART_SRCS})

target_link_libraries(
    calligra_filter_pptx2odp
    komsooxml
    mso
    koodf2
    koodf
    komain
)

install(TARGETS calligra_filter_pptx2odp DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

########### install files ###############

if(SHOULD_BUILD_OKULAR_GENERATOR_PPTX)
    kcoreaddons_add_plugin(okularGenerator_pptx_calligra
        INSTALL_NAMESPACE "okular_generators"
        SOURCES OkularPptxGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_pptx_calligra
        kookularGenerator_odp
    )

    install( PROGRAMS okularApplication_pptx_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
