
include_directories( ${POPPLER_INCLUDE_DIR} )

macro_optional_find_package(OpenJPEG)
macro_optional_find_package(JPEG)

set(pdf2svg_PART_SRCS PdfImport.cpp SvgOutputDev.cpp )

kde4_add_plugin(calligra_filter_pdf2svg ${pdf2svg_PART_SRCS})

target_link_libraries(calligra_filter_pdf2svg komain ${POPPLER_LIBRARY} ${POPPLER_CORE_LIBRARY})
if(JPEG_FOUND)
    target_link_libraries(calligra_filter_pdf2svg jpeg)
endif(JPEG_FOUND)
if(OPENJPEG_FOUND)
    target_link_libraries(calligra_filter_pdf2svg ${OPENJPEG_LIBRARIES})
endif(OPENJPEG_FOUND)

install(TARGETS calligra_filter_pdf2svg DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES karbon_pdf_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
