set(pdf2svg_PART_SRCS PdfImportDebug.cpp PdfImport.cpp SvgOutputDev.cpp )

add_library(calligra_filter_pdf2svg MODULE ${pdf2svg_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_pdf2svg calligra_filter_pdf2svg.desktop)

target_link_libraries(calligra_filter_pdf2svg komain Poppler::Core)

install(TARGETS calligra_filter_pdf2svg DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

# pdf to odg
set(pdf2odg_PART_SRCS PdfImportDebug.cpp Pdf2OdgImport.cpp SvgOutputDev.cpp)

add_library(calligra_filter_pdf2odg MODULE ${pdf2odg_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_pdf2odg calligra_filter_pdf2odg.desktop)

target_link_libraries(calligra_filter_pdf2odg kopageapp karbonui Poppler::Core)

install(TARGETS calligra_filter_pdf2odg DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
