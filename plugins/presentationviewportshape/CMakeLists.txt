include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/plugins/pathshapes ${CMAKE_SOURCE_DIR}/libs/widgets)

#add_subdirectory(pics)

SET (presentationviewportshape_PART_SRCS
    ${CMAKE_SOURCE_DIR}/libs/widgets/KoUnitDoubleSpinBox.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShapeFactory.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShapeConfigWidget.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShapeConfigCommand.cpp
    PresentationViewPortShape.cpp
    ViewPortShapeConfigWidget.cpp
    ViewPortShapeConfigCommand.cpp
    ViewPortShapeFactory.cpp 
    ViewPortTool.cpp
    )

kde4_add_ui_files(presentationviewportshape_PART_SRCS
    ViewPortShapeConfigWidget.ui
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShapeConfigWidget.ui
)
kde4_add_plugin(presentationviewportshape ${presentationviewportshape_PART_SRCS})

target_link_libraries(presentationviewportshape flake)

install(TARGETS presentationviewportshape DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES presentationviewportshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
