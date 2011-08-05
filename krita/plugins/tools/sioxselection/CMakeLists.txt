

set(kritatoolsioxselection_PART_SRCS
    tool_sioxselection.cc
    kis_tool_sioxselection.cc
    )

kde4_add_plugin(kritatoolsioxselection ${kritatoolsioxselection_PART_SRCS})



target_link_libraries(kritatoolsioxselection kritaui)

install(TARGETS kritatoolsioxselection  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

# install( FILES  tool_sioxselection.png  DESTINATION ${DATA_INSTALL_DIR}/krita/pics)
install( FILES  kritatoolsioxselection.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


