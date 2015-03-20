
include_directories( ${CMAKE_SOURCE_DIR}/karbon
                     ${CMAKE_SOURCE_DIR}/karbon/core
                      )


if(SHOULD_BUILD_FILTER_KARBON_TO_SVG)

set(karbon2svg_PART_SRCS
    SvgExport.cpp
)

add_library(calligra_filter_karbon2svg MODULE ${karbon2svg_PART_SRCS})

target_link_libraries(calligra_filter_karbon2svg karbonui)

install(TARGETS calligra_filter_karbon2svg DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_karbon2svg.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

endif()



if(SHOULD_BUILD_FILTER_SVG_TO_KARBON)

set(svg2karbon_PART_SRCS
    SvgImport.cpp
)

add_library(calligra_filter_svg2karbon MODULE ${svg2karbon_PART_SRCS})

target_link_libraries(calligra_filter_svg2karbon karbonui)

install(TARGETS calligra_filter_svg2karbon  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_svg2karbon.desktop calligra_filter_svgz2karbon.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

endif()
