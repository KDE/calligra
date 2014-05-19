
#set(KRITA_PRESETDOCKER_PART_SRCS presetdocker.cpp presetdocker_dock.cpp )
set(KRITA_PREVIEWRENDERDOCKER_PART_SRCS previewrenderdocker.cpp previewrenderdocker_dock.cpp kis_previewrenderer_widget.cpp )

kde4_add_plugin(kritapreviewrenderdocker ${KRITA_PREVIEWRENDERDOCKER_PART_SRCS})

target_link_libraries(kritapreviewrenderdocker kritaui)

install(TARGETS kritapreviewrenderdocker  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_previewrenderdocker.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

