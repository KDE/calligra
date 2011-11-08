
set(KRITA_ANIMATORDOCKER_PART_SRCS
    animator.cpp animator_manager_factory.cpp
    animator_manager.cpp animator_frame_manager.cpp animator_switcher.cpp
    animator_player.cpp animator_exporter.cpp animator_importer.cpp
    animator_updater.cpp animator_lt_updater.cpp
    animator_loader.cpp animator_meta_info.cpp
    animator_model.cpp animator_view.cpp
    animator_actions.cpp
    animator_dock.cpp animator_control_dock.cpp animator_lt_dock.cpp
    animator_lt.cpp animator_lt_view.cpp lt_slider.cpp
    
    animated_layer.cpp frame_layer.cpp
    framed_animated_layer.cpp simple_frame_layer.cpp
    interpolated_animated_layer.cpp normal_animated_layer.cpp
    abstract_interpolation.cpp moving_interpolation.cpp transparency_interpolation.cpp
    control_animated_layer.cpp control_frame_layer.cpp
    view_animated_layer.cpp
)

kde4_add_plugin(kritaanimatordocker ${KRITA_ANIMATORDOCKER_PART_SRCS})

target_link_libraries(kritaanimatordocker kritaui)

install(TARGETS kritaanimatordocker DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animatordocker.desktop DESTINATION ${SERVICES_INSTALL_DIR})

