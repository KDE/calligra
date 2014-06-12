include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} )

add_subdirectory(pics)

########### Flake Plugin library ###############

SET (variablelineshape_SRCS
    VariableLineShape.cpp
    VariableLineShapeShapeFactory.cpp
    VariableLineShape.cpp
)

kde4_add_plugin(calligra_shape_variableline ${variablelineshape_SRCS})

target_link_libraries(calligra_shape_variableline flake kowidgets)

install(TARGETS calligra_shape_variableline DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES calligra_shape_variableline.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )