if(SHOULD_BUILD_FILTER_ODT_TO_DOCX)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
     ${KOODF2_INCLUDES}
)

set(Odt2Docx_PART_SRCS
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
kde4_add_plugin(calligra_filter_odt2docx ${Odt2Docx_PART_SRCS})
target_link_libraries(calligra_filter_odt2docx koodfreader komain koodf koodf2)
install(TARGETS calligra_filter_odt2docx DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES calligra_filter_odt2docx.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

endif(SHOULD_BUILD_FILTER_ODT_TO_DOCX)
