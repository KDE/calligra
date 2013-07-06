
include_directories( ${CMAKE_SOURCE_DIR}/karbon
                     ${CMAKE_SOURCE_DIR}/karbon/core
                     ${KDE4_INCLUDES} )


if(SHOULD_BUILD_FILTER_KARBON_TO_SVG)

set(karbon2svg_PART_SRCS
    SvgExport.cpp
)

kde4_add_plugin(calligra_filter_karbon2svg ${karbon2svg_PART_SRCS})

target_link_libraries(calligra_filter_karbon2svg karbonui)

install(TARGETS calligra_filter_karbon2svg DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  karbon_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_KARBON_TO_SVG)



if(SHOULD_BUILD_FILTER_SVG_TO_KARBON)

set(svg2karbon_PART_SRCS
    SvgImport.cpp
)

kde4_add_plugin(calligra_filter_svg2karbon ${svg2karbon_PART_SRCS})

target_link_libraries(calligra_filter_svg2karbon karbonui)

install(TARGETS calligra_filter_svg2karbon  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  karbon_svg_import.desktop karbon_svgz_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SVG_TO_KARBON)
