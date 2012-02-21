########### next target ###############
include_directories( ${POPPLER_INCLUDE_DIR} )

set(PdfImport_PART_SRCS PdfImport.cpp OdfOutputDev.cpp ImportDialog.cc )

kde4_add_ui_files(PdfImport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(pdfimport ${PdfImport_PART_SRCS})

target_link_libraries(pdfimport komain poppler poppler-qt4)

install(TARGETS pdfimport  DESTINATION ${PLUGIN_INSTALL_DIR})



########### install files ###############

install( FILES  words_pdf_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

#add_subdirectory(poppler)

