set(kritasandpaintop_PART_SRCS
    sand_paintop_plugin.cpp
    kis_sand_paintop.cpp
    kis_sandop_option.cpp
    kis_sand_paintop_settings.cpp
    kis_sand_paintop_settings_widget.cpp
    sand_brush.cpp
    particle.cpp
    )

kde4_add_ui_files(kritasandpaintop_PART_SRCS wdgsandoptions.ui )

kde4_add_plugin(kritasandpaintop ${kritasandpaintop_PART_SRCS})


target_link_libraries(kritasandpaintop kritalibpaintop)

install(TARGETS kritasandpaintop  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritasandpaintop.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES
    krita-sand.png DESTINATION ${DATA_INSTALL_DIR}/krita/images)
install( FILES  3d-pencil.obj DESTINATION ${DATA_INSTALL_DIR}/krita/brushmodels )
install( FILES  3d-pencil.mtl DESTINATION ${DATA_INSTALL_DIR}/krita/brushmodels )


