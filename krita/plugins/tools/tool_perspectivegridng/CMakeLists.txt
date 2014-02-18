

set(kritatoolperspectivegridng_PART_SRCS
    tool_perspectivegridng.cc
    kis_tool_perspectivegridng.cc
    )

kde4_add_ui_files(kritatoolperspectivegridng_PART_SRCS PerspectiveGridNgToolOptions.ui )

kde4_add_plugin(kritatoolperspectivegridng ${kritatoolperspectivegridng_PART_SRCS})

target_link_libraries(kritatoolperspectivegridng kritaui flake ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} )

install(TARGETS kritatoolperspectivegridng  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  tool_perspectivegridng.png DESTINATION ${DATA_INSTALL_DIR}/krita/pics)
install( FILES  kritatoolperspectivegridng.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


