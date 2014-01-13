

set(kritatoolperspectivegridng_PART_SRCS
    tool_perspectivegridng.cc
    kis_tool_perspectivegridng.cc
    )

kde4_add_plugin(kritatoolperspectivegridng ${kritatoolperspectivegridng_PART_SRCS})



target_link_libraries(kritatoolperspectivegridng kritaui)

install(TARGETS kritatoolperspectivegridng  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  tool_perspectivegridng.png tool_perspectivegrid_cursorng.png  DESTINATION ${DATA_INSTALL_DIR}/krita/pics)
install( FILES  kritatoolperspectivegridng.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


