project(karbondockersplugin)

include_directories( 
    ${KDE4_INCLUDE_DIR} 
    ${QT_INCLUDES} 
    ${KOGUIUTILS_INCLUDES}
    ${CMAKE_SOURCE_DIR}/karbon/ui/widgets )

set(karbondockers_SRCS
    KarbonDockersPlugin.cpp
    KarbonStyleDocker.cpp
    KarbonStyleDockerFactory.cpp
)

kde4_add_plugin(karbondockersplugin ${karbondockers_SRCS})

target_link_libraries(karbondockersplugin karboncommon koresources pigmentcms flake ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDEUI_LIBS} koguiutils komain karbonui )

install(TARGETS karbondockersplugin DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES karbondockersplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
