include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} )

set(pathshapes_PART_SRCS
    regularpolygon/KoRegularPolygonShape.cpp
    regularpolygon/KoRegularPolygonShapeFactory.cpp
    star/KoStarShape.cpp
    star/KoStarShapeFactory.cpp
    rectangle/KoRectangleShape.cpp
    rectangle/KoRectangleShapeFactory.cpp
    KoPathShapesPlugin.cpp
)

kde4_automoc(${pathshapes_PART_SRCS})

kde4_add_plugin(pathshapes ${pathshapes_PART_SRCS})

target_link_libraries(pathshapes flake kofficeui)

install(TARGETS pathshapes  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES pathshapes.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
