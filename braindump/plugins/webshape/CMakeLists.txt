include_directories (${FLAKE_INCLUDES} )

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

target_link_libraries(
    braindump_shape_web
    flake
    Qt5::Core
    Qt5::WebKitWidgets
    Qt5::Svg
    )
install(TARGETS braindump_shape_web DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

calligra_shape_desktop_to_json(braindump_shape_web braindump_shape_web.desktop)
