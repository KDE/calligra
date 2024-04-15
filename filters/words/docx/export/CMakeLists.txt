if(SHOULD_BUILD_FILTER_ODT_TO_DOCX)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
     ${KOODF2_INCLUDES}
)

set(Odt2Docx_PART_SRCS
    DocxExportDebug.cpp
    DocxExport.cpp
    DocxStyleWriter.cpp
    DocxStyleHelper.cpp
    OdfReaderDocxContext.cpp
    OdtReaderDocxBackend.cpp
    OdfTextReaderDocxBackend.cpp
    FileCollector.cpp
    DocxFile.cpp
    UnitConversions.cpp
    OpcContentTypes.cpp
    OpcRelSet.cpp
    OpcRelSetManager.cpp
)
add_library(calligra_filter_odt2docx MODULE ${Odt2Docx_PART_SRCS})
target_link_libraries(calligra_filter_odt2docx koodfreader komain koodf koodf2)
install(TARGETS calligra_filter_odt2docx DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

#install(FILES calligra_filter_odt2docx.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/calligra)

endif()
