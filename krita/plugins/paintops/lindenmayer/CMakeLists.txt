set(kritalindenmayerpaintop_PART_SRCS
    lindenmayer_paintop_plugin.cpp
    kis_lindenmayer_paintop.cpp
    kis_lindenmayerop_option.cpp
    kis_lindenmayer_paintop_settings.cpp
    kis_lindenmayer_paintop_settings_widget.cpp
    kis_lindenmayer_letter.cpp
    kis_lindenmayer_production.cpp
    lindenmayer_brush.cpp	
    )

kde4_add_ui_files(kritalindenmayerpaintop_PART_SRCS wdglindenmayeroptions.ui )

kde4_add_plugin(kritalindenmayerpaintop ${kritalindenmayerpaintop_PART_SRCS})


target_link_libraries(kritalindenmayerpaintop kritalibpaintop)

install(TARGETS kritalindenmayerpaintop  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritalindenmayerpaintop.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES
    krita-lindenmayer.png DESTINATION ${DATA_INSTALL_DIR}/krita/images)
install( FILES  3d-pencil.obj DESTINATION ${DATA_INSTALL_DIR}/krita/brushmodels )
install( FILES  3d-pencil.mtl DESTINATION ${DATA_INSTALL_DIR}/krita/brushmodels )


