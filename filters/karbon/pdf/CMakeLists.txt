
include_directories( ${POPPLER_INCLUDE_DIR} )

macro_optional_find_package(OpenJPEG)
macro_optional_find_package(JPEG)

set(pdf2svg_PART_SRCS PdfImportDebug.cpp PdfImport.cpp SvgOutputDev.cpp )

add_library(calligra_filter_pdf2svg MODULE ${pdf2svg_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_pdf2svg calligra_filter_pdf2svg.desktop)

target_link_libraries(calligra_filter_pdf2svg komain ${POPPLER_LIBRARY} ${POPPLER_CORE_LIBRARY})
if(JPEG_FOUND)
    target_link_libraries(calligra_filter_pdf2svg jpeg)
endif()
if(OPENJPEG_FOUND)
    target_link_libraries(calligra_filter_pdf2svg ${OPENJPEG_LIBRARIES})
endif()

install(TARGETS calligra_filter_pdf2svg DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
