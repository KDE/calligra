set(kritastackedpaintop_PART_SRCS
    stacked_paintop_plugin.cpp
    stacked_paintop_settings.cpp
    stacked_paintop_settings_widget.cpp
    stacked_brush.cpp	
    stacked_paintop.cpp
    stackedop_option.cpp
)

kde4_add_ui_files(kritastackedpaintop_PART_SRCS wdgstackedoptions.ui )

kde4_add_plugin(kritastackedpaintop ${kritastackedpaintop_PART_SRCS})


target_link_libraries(kritastackedpaintop  kritaui kritalibpaintop kritalibbrush kowidgetutils)

install(TARGETS kritastackedpaintop  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kritastackedpaintop.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install( FILES
    krita-stacked.png DESTINATION ${DATA_INSTALL_DIR}/krita/images)
