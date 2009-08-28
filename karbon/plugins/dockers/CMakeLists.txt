project(karbondockersplugin)

include_directories( 
    
    ${KOPAGEAPP_INCLUDES}
    ${CMAKE_SOURCE_DIR}/karbon/ui/widgets )

set(karbondockers_SRCS
    KarbonDockersPlugin.cpp
    KarbonStyleDocker.cpp
    KarbonStyleDockerFactory.cpp
)

kde4_add_plugin(karbondockersplugin ${karbondockers_SRCS})

target_link_libraries(karbondockersplugin karboncommon kocolorwidgets koresourcewidget karbonui )

install(TARGETS karbondockersplugin DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES karbondockersplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
