
include_directories( ${POPPLER_INCLUDE_DIR} )

macro_optional_find_package(OpenJPEG)
macro_optional_find_package(JPEG)

set(karbonpdfimport_PART_SRCS PdfImport.cpp SvgOutputDev.cpp )

kde4_add_plugin(karbonpdfimport WITH_PREFIX ${karbonpdfimport_PART_SRCS})

target_link_libraries(karbonpdfimport komain poppler poppler-qt4)
if(JPEG_FOUND)
    target_link_libraries(karbonpdfimport jpeg)
endif(JPEG_FOUND)
if(OPENJPEG_FOUND)
    target_link_libraries(karbonpdfimport ${OPENJPEG_LIBRARIES})
endif(OPENJPEG_FOUND)

install(TARGETS karbonpdfimport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES karbon_pdf_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
