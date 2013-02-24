
include_directories( ${CMAKE_SOURCE_DIR}/stage
                     ${CMAKE_SOURCE_DIR}/stage/core
                     ${KDE4_INCLUDES})

########### next target ###############

set(svgsinglecanvasexport_PART_SRCS 
    SvgPresExport.cpp
)

kde4_add_plugin(svgsinglecanvasexport ${svgsinglecanvasexport_PART_SRCS})

target_link_libraries(svgsinglecanvasexport komain)

install(TARGETS svgsinglecanvasexport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  svg_singlecanvas_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
