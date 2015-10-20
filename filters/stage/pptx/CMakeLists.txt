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
    ${CMAKE_SOURCE_DIR}/filters/libodf2/chart/KoOdfChartWriter.cpp
    ${CMAKE_SOURCE_DIR}/filters/sheets/xlsx/XlsxXmlChartReader.cpp
)

add_library(calligra_filter_pptx2odp MODULE ${pptx2odp_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_pptx2odp calligra_filter_pptx2odp.desktop)

target_link_libraries(
    calligra_filter_pptx2odp
    komsooxml
    mso
    koodf2
    koodf
    komain
)

install(TARGETS calligra_filter_pptx2odp DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES stage_pptx_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULARGENERATOR_PPTX)
    install( FILES libokularGenerator_pptx_calligra.desktop okularPptx_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_pptx_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
