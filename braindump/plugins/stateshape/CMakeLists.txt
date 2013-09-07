include_directories (${FLAKE_INCLUDES} )

########### Flake Plugin library ###############

SET (stateshape_LIB_SRCS
    CategorizedItemDelegate.cpp
    StatesModel.cpp
    StateShape.cpp
    StateShapeChangeStateCommand.cpp
    StateShapeFactory.cpp
    StateShapePlugin.cpp
    StateTool.cpp
    StateToolFactory.cpp
    StateToolWidget.cpp
)

kde4_add_ui_files( stateshape_LIB_SRCS
    StateShapeConfigWidget.ui
)

kde4_add_plugin(braindump_shape_state ${stateshape_LIB_SRCS})

target_link_libraries(
    braindump_shape_state
    flake
    ${QT_QTCORE_LIBRARY}
    braindumpcore
    )
install(TARGETS braindump_shape_state DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES braindump_shape_state.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )
