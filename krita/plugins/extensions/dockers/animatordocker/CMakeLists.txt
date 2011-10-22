
set(KRITA_ANIMATORDOCKER_PART_SRCS
    animator.cpp animator_manager_factory.cpp
    animator_manager.cpp animator_loader.cpp animator_switcher.cpp
    animator_model.cpp animator_view.cpp
    animator_dock.cpp animator_control_dock.cpp
    
    animated_layer.cpp framed_animated_layer.cpp
    frame_layer.cpp simple_frame_layer.cpp
)

kde4_add_plugin(kritaanimatordocker ${KRITA_ANIMATORDOCKER_PART_SRCS})

target_link_libraries(kritaanimatordocker kritaui)

install(TARGETS kritaanimatordocker DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animatordocker.desktop DESTINATION ${SERVICES_INSTALL_DIR})

