
set(KRITA_ANIMATORDOCKER_PART_SRCS animator_model.cpp animator_view.cpp animator_exporter.cpp animator_player.cpp animator.cpp animator_dock.cpp animator_light_table_dock.cpp animator_light_table.cpp ltslider.cpp frame_layer.cpp animated_layer.cpp simple_animated_layer.cpp interpolated_animated_layer.cpp normal_animated_layer.cpp abstract_interpolation.cpp moving_interpolation.cpp transparency_interpolation.cpp)

kde4_add_plugin(kritaanimatordocker ${KRITA_ANIMATORDOCKER_PART_SRCS})

target_link_libraries(kritaanimatordocker kritaui)

install(TARGETS kritaanimatordocker DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animatordocker.desktop DESTINATION ${SERVICES_INSTALL_DIR})

