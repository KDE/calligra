include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} )

add_subdirectory(pics)

set(pathshapes_PART_SRCS
    ellipse/KoEllipseShape.cpp
    ellipse/KoEllipseShapeFactory.cpp
    regularpolygon/KoRegularPolygonShape.cpp
    regularpolygon/KoRegularPolygonShapeFactory.cpp
    star/KoStarShape.cpp
    star/KoStarShapeFactory.cpp
    rectangle/KoRectangleShape.cpp
    rectangle/KoRectangleShapeFactory.cpp
    arrow/KoArrowShape.cpp
    arrow/KoArrowShapeFactory.cpp
    enhancedpath/KoEnhancedPathShape.cpp
    enhancedpath/KoEnhancedPathShapeFactory.cpp
    enhancedpath/KoEnhancedPathCommand.cpp
    enhancedpath/KoEnhancedPathParameter.cpp
    enhancedpath/KoEnhancedPathFormula.cpp
    enhancedpath/KoEnhancedPathHandle.cpp
    PathShapesPlugin.cpp
)

kde4_automoc(${pathshapes_PART_SRCS})

kde4_add_plugin(pathshapes ${pathshapes_PART_SRCS})

target_link_libraries(pathshapes flake kofficeui)

install(TARGETS pathshapes  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES pathshapes.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
