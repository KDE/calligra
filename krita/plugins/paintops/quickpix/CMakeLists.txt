set(kritaquickpixpaintop_PART_SRCS
    quickpix_paintop_plugin.cpp
    kis_quickpix_paintop.cpp
    kis_quickpix_paintop_options.cpp
    kis_quickpix_paintop_settings.cpp
    kis_quickpix_paintop_settings_widget.cpp
    )

kde4_add_ui_files(kritaquickpixpaintop_PART_SRCS wdgquickpixoptions.ui )

kde4_add_plugin(kritaquickpixpaintop ${kritaquickpixpaintop_PART_SRCS})


target_link_libraries(kritaquickpixpaintop kritalibpaintop)

install(TARGETS kritaquickpixpaintop  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaquickpixpaintop.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES
    krita-quickpix.png DESTINATION ${DATA_INSTALL_DIR}/krita/images)

