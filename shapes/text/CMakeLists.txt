project( textPlugin)

add_subdirectory( tests )

include_directories( ${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES} )

########### Flake Plugin library ###############

SET ( textshape_SRCS
    TextPlugin.cpp
    TextShape.cpp
    TextShapeFactory.cpp
    TextTool.cpp
    TextToolFactory.cpp
    Layout.cpp
    ListItemsHelper.cpp
    dialogs/StylesWidget.cpp
    dialogs/SimpleStyleWidget.cpp
)

kde4_add_ui_files(textshape_SRCS
    dialogs/SimpleStyleWidget.ui
    dialogs/StylesWidget.ui
)

kde4_automoc(${textshape_SRCS})
kde4_add_plugin(textshape ${textshape_SRCS})

target_link_libraries(textshape flake kofficeui kotext)
install(TARGETS textshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES textshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
