
include_directories( ${CMAKE_SOURCE_DIR}/karbon )


if(SHOULD_BUILD_FILTER_KARBON_TO_SVG)

set(karbon2svg_PART_SRCS
    SvgExport.cpp
)

add_library(calligra_filter_karbon2svg MODULE ${karbon2svg_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_karbon2svg calligra_filter_karbon2svg.desktop)

target_link_libraries(calligra_filter_karbon2svg karbonui kopageapp)

install(TARGETS calligra_filter_karbon2svg DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()



if(SHOULD_BUILD_FILTER_SVG_TO_KARBON)

set(svg2karbon_PART_SRCS
    SvgImport.cpp
)

add_library(calligra_filter_svg2karbon MODULE ${svg2karbon_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_svg2karbon calligra_filter_svg2karbon.desktop)

target_link_libraries(calligra_filter_svg2karbon karbonui kopageapp KF5::Archive)

install(TARGETS calligra_filter_svg2karbon  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()
