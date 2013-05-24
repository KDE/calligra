
include_directories( ${CMAKE_SOURCE_DIR}/karbon
                     ${CMAKE_SOURCE_DIR}/karbon/core
                     ${KDE4_INCLUDES} )


if(SHOULD_BUILD_FILTER_KARBON_TO_SVG)

set(karbonsvgexport_PART_SRCS 
    SvgExport.cpp
)

kde4_add_plugin(karbonsvgexport ${karbonsvgexport_PART_SRCS})

target_link_libraries(karbonsvgexport karbonui)

install(TARGETS karbonsvgexport DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  karbon_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_KARBON_TO_SVG)



if(SHOULD_BUILD_FILTER_SVG_TO_KARBON)

set(karbonsvgimport_PART_SRCS 
    SvgImport.cpp
)

kde4_add_plugin(karbonsvgimport ${karbonsvgimport_PART_SRCS})

target_link_libraries(karbonsvgimport karbonui)

install(TARGETS karbonsvgimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  karbon_svg_import.desktop karbon_svgz_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SVG_TO_KARBON)
