add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_plugin\")

include_directories( ${KOMAIN_INCLUDES} )

########### Flake Plugin library ###############

set (pluginshape_LIB_SRCS
    PluginShape.cpp
    PluginShapeFactory.cpp
    Plugin.cpp
)

add_library(calligra_shape_plugin MODULE ${pluginshape_LIB_SRCS})

target_link_libraries(calligra_shape_plugin
    komain
)
install(TARGETS calligra_shape_plugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
