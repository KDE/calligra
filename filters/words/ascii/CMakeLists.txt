if(SHOULD_BUILD_FILTER_ASCII_TO_WORDS)

include_directories(
    ${CMAKE_SOURCE_DIR}/words/part
)


set(AsciiImport_PART_SRCS
    AsciiImport.cpp
    ImportDialog.cc
)
kde4_add_ui_files(AsciiImport_PART_SRCS ImportDialogUI.ui )
kde4_add_plugin(asciiimport ${AsciiImport_PART_SRCS})
target_link_libraries(asciiimport komain textlayout wordsprivate)
install(TARGETS asciiimport DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES words_ascii_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ASCII_TO_WORDS)


if(SHOULD_BUILD_FILTER_ODT_TO_ASCII)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodftraverse
)

set(AsciiExport_PART_SRCS
    AsciiExport.cpp
    OdtTraverserAsciiBackend.cpp
)
kde4_add_plugin(asciiexport ${AsciiExport_PART_SRCS})
target_link_libraries(asciiexport odftraverse odfreader komain koodf)
install(TARGETS asciiexport DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES words_ascii_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ODT_TO_ASCII)
