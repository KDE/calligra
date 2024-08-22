
include_directories(${CMAKE_BINARY_DIR}/filters/ ${LIBWPG_INCLUDE_DIR}/)

set(wpg2svg_PART_SRCS WPGImport.cpp)

add_library(calligra_filter_wpg2svg MODULE ${wpg2svg_PART_SRCS})

target_link_libraries(calligra_filter_wpg2svg komain ${LIBWPG_LIBRARIES} LibRevenge::LibRevenge LibRevenge::LibRevengeStream)

install(TARGETS calligra_filter_wpg2svg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

