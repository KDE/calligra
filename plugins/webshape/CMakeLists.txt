project(webshape)

include_directories (${FLAKE_INCLUDES} )

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_webshape\")

########### Flake Plugin library ###############

set (webshape_LIB_SRCS
    WebShape.cpp
    WebShapeFactory.cpp
    WebShapePlugin.cpp
    WebTool.cpp
    WebToolWidget.cpp
    WebToolFactory.cpp
)

ki18n_wrap_ui( webshape_LIB_SRCS
    WebShapeConfigWidget.ui
)

add_library(braindump_shape_web MODULE ${webshape_LIB_SRCS})

target_link_libraries(braindump_shape_web PRIVATE
    flake
    Qt6::Core
    Qt6::WebEngineWidgets
    Qt6::Svg
)
install(TARGETS braindump_shape_web DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

