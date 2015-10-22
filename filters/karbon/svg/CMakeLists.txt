
include_directories( ${CMAKE_SOURCE_DIR}/karbon
                     ${CMAKE_SOURCE_DIR}/karbon/core
                      )


if(SHOULD_BUILD_FILTER_KARBON_TO_SVG)

set(karbon2svg_PART_SRCS
    SvgExport.cpp
)

add_library(calligra_filter_karbon2svg MODULE ${karbon2svg_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_karbon2svg calligra_filter_karbon2svg.desktop)

target_link_libraries(calligra_filter_karbon2svg karbonui)

install(TARGETS calligra_filter_karbon2svg DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

endif()



if(SHOULD_BUILD_FILTER_SVG_TO_KARBON)

set(svg2karbon_PART_SRCS
    SvgImport.cpp
)

add_library(calligra_filter_svg2karbon MODULE ${svg2karbon_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_svg2karbon calligra_filter_svg2karbon.desktop)

target_link_libraries(calligra_filter_svg2karbon karbonui KF5::Archive KF5::KDELibs4Support)

install(TARGETS calligra_filter_svg2karbon  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

endif()
