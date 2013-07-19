if(SHOULD_BUILD_FILTER_ODT_TO_DOCX)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
     ${KOODF2_INCLUDES}
)

set(DocxExport_PART_SRCS
    DocxExport.cpp
    OdfReaderDocxContext.cpp
    OdtReaderDocxBackend.cpp
    FileCollector.cpp
    DocxFile.cpp

    OpcContentTypes.cpp
    OpcRelSet.cpp
    OpcRelSetManager.cpp
)
kde4_add_plugin(docxexport ${DocxExport_PART_SRCS})
target_link_libraries(docxexport koodfreader komain koodf koodf2)
install(TARGETS docxexport DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES words_docx_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ODT_TO_DOCX)
