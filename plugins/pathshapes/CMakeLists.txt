add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_paths\")

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/widgets
    ${FLAKE_INCLUDES}
)

add_subdirectory(pics)

set(pathshapes_PART_SRCS)

ki18n_wrap_ui(pathshapes_PART_SRCS
    star/StarShapeConfigWidget.ui
    rectangle/RectangleShapeConfigWidget.ui
    ellipse/EllipseShapeConfigWidget.ui
    spiral/SpiralShapeConfigWidget.ui
)
add_library(calligra_shape_paths MODULE
    ${pathshapes_PART_SRCS}
    ellipse/EllipseShape.cpp
    ellipse/EllipseShapeFactory.cpp
    ellipse/EllipseShapeConfigWidget.cpp
    ellipse/EllipseShapeConfigCommand.cpp
    spiral/SpiralShape.cpp
    spiral/SpiralShapeFactory.cpp
    spiral/SpiralShapeConfigWidget.cpp
    spiral/SpiralShapeConfigCommand.cpp
    star/StarShape.cpp
    star/StarShapeFactory.cpp
    star/StarShapeConfigWidget.cpp
    star/StarShapeConfigCommand.cpp
    rectangle/RectangleShape.cpp
    rectangle/RectangleShapeFactory.cpp
    rectangle/RectangleShapeConfigWidget.cpp
    rectangle/RectangleShapeConfigCommand.cpp
    enhancedpath/EnhancedPathShape.cpp
    enhancedpath/EnhancedPathShapeFactory.cpp
    enhancedpath/EnhancedPathCommand.cpp
    enhancedpath/EnhancedPathParameter.cpp
    enhancedpath/EnhancedPathFormula.cpp
    enhancedpath/EnhancedPathHandle.cpp
    enhancedpath/CalloutShape.cpp
    enhancedpath/CalloutShapeFactory.cpp
    enhancedpath/CalloutContainerModel.cpp
    enhancedpath/CalloutToolFactory.cpp
    enhancedpath/CalloutDebug.cpp
    PathShapesPlugin.cpp
)

target_link_libraries(calligra_shape_paths flake kowidgets)

install(TARGETS calligra_shape_paths  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
