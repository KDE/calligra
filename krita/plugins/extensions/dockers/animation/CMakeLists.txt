add_subdirectory(tests)

set(KRITA_ANIMATIONDOCKER_PART_SRCS
    animation_dockers.cpp
    animation_docker.cpp
    timeline_docker.cpp
    onion_skins_docker.cpp

    timeline_layers_header.cpp
    timeline_ruler_header.cpp

    timeline_frames_model.cpp
    timeline_frames_view.cpp
    timeline_frames_item_delegate.cpp

    timeline_frames_index_converter.cpp
    timeline_node_list_keeper.cpp

    timeline_color_scheme.cpp

    kis_draggable_tool_button.cpp
    kis_animation_utils.cpp
    kis_custom_modifiers_catcher.cpp

    kis_equalizer_column.cpp
    kis_equalizer_slider.cpp
    kis_equalizer_button.cpp
    kis_equalizer_widget.cpp
)

kde4_add_ui_files(KRITA_ANIMATIONDOCKER_PART_SRCS
    wdg_animation.ui
    onion_skins_docker.ui
)

kde4_add_plugin(kritaanimationdocker ${KRITA_ANIMATIONDOCKER_PART_SRCS})

target_link_libraries(kritaanimationdocker kritaui)

install(TARGETS kritaanimationdocker  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animationdocker.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

