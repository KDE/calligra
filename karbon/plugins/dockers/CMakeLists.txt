project(karbondockersplugin)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} )

set(karbondockers_SRCS
    KarbonDockersPlugin.cpp
    KarbonStrokeDocker.cpp
    KarbonStrokeDockerFactory.cpp
)

kde4_add_plugin(karbondockersplugin ${karbondockers_SRCS})

target_link_libraries(karbondockersplugin koguiutils komain)

install(TARGETS karbondockersplugin DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES karbondockersplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
