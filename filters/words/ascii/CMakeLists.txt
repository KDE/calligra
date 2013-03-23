include_directories(
    ${CMAKE_SOURCE_DIR}/words/part              # for the import filter
    ${CMAKE_SOURCE_DIR}/filters/libodftraverse  # for the export filter
)

# ########### next target ###############

set(AsciiImport_PART_SRCS
    AsciiImport.cpp
    ImportDialog.cc
)
kde4_add_ui_files(AsciiImport_PART_SRCS ImportDialogUI.ui )
kde4_add_plugin(asciiimport ${AsciiImport_PART_SRCS})
target_link_libraries(asciiimport komain textlayout wordsprivate)
install(TARGETS asciiimport DESTINATION ${PLUGIN_INSTALL_DIR})

# ########### next target ###############

set(AsciiExport_PART_SRCS
    AsciiExport.cpp
    OdtTraverserAsciiBackend.cpp 
)
kde4_add_plugin(asciiexport ${AsciiExport_PART_SRCS})
target_link_libraries(asciiexport odftraverse komain koodf)
install(TARGETS asciiexport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES words_ascii_import.desktop words_ascii_export.desktop
         DESTINATION ${SERVICES_INSTALL_DIR})
