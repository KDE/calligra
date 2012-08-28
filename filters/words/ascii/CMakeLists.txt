include_directories( ${CMAKE_SOURCE_DIR}/words/part )

# ########### next target ###############

set(AsciiImport_PART_SRCS AsciiImport.cpp ImportDialog.cc )
kde4_add_ui_files(AsciiImport_PART_SRCS ImportDialogUI.ui )
kde4_add_plugin(asciiimport ${AsciiImport_PART_SRCS})
target_link_libraries(asciiimport komain textlayout wordsprivate)
install(TARGETS asciiimport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  words_ascii_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
