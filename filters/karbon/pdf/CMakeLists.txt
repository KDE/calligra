
include_directories( ${POPPLER_INCLUDE_DIR} )

set(karbonpdfimport_PART_SRCS PdfImport.cpp SvgOutputDev.cpp )

kde4_add_plugin(karbonpdfimport WITH_PREFIX ${karbonpdfimport_PART_SRCS})

target_link_libraries(
    karbonpdfimport  
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    poppler 
    poppler-qt4 )

install(TARGETS karbonpdfimport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES karbon_pdf_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
