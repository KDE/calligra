include_directories( ${CMAKE_SOURCE_DIR}/stage
                     ${CMAKE_SOURCE_DIR}/stage/part
                     ${KDE4_INCLUDES})

########### next target ###############

set(svgsinglecanvasexport_PART_SRCS 
    SvgPresExport.cpp
)

kde4_add_plugin(svgsinglecanvasexport ${svgsinglecanvasexport_PART_SRCS})
# TODO Allthese libs needed?
target_link_libraries(svgsinglecanvasexport komain kopageapp calligrastageprivate)

install(TARGETS svgsinglecanvasexport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  svg_singlecanvas_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
