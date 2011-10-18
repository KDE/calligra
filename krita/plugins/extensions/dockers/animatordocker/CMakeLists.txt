
set(KRITA_ANIMATORDOCKER_PART_SRCS animator.cpp animator_model.cpp animator_model_factory.cpp animator_dock.cpp )

kde4_add_plugin(kritaanimatordocker ${KRITA_ANIMATORDOCKER_PART_SRCS})

target_link_libraries(kritaanimatordocker kritaui)

install(TARGETS kritaanimatordocker DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animatordocker.desktop DESTINATION ${SERVICES_INSTALL_DIR})

