include_directories( ${CMAKE_SOURCE_DIR}/plugins/pathshapes  ${KDE4_INCLUDES}  )

########### next target ###############

set(SHAPES_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/ellipse/EllipseShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/star/StarShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShapeLoadingUpdater.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextRange.cpp
)

set(svgexport_PART_SRCS 
    Frame.cpp
    SvgWriter_generic.cpp
    ${SHAPES_SOURCES} 
)


kde4_add_plugin(svgexport ${svgexport_PART_SRCS})

install(TARGETS svgexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  svg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
