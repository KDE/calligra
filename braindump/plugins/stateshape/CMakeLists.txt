include_directories (${FLAKE_INCLUDES} )

########### Flake Plugin library ###############

set (stateshape_LIB_SRCS
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

ki18n_wrap_ui( stateshape_LIB_SRCS
    StateShapeConfigWidget.ui
)

add_library(braindump_shape_state MODULE ${stateshape_LIB_SRCS})

target_link_libraries(
    braindump_shape_state
    flake
    Qt5::Core
    Qt5::Svg
    braindumpcore
    KF5::ItemViews
    KF5::CoreAddons
    )
install(TARGETS braindump_shape_state DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

calligra_shape_desktop_to_json(braindump_shape_state braindump_shape_state.desktop)
