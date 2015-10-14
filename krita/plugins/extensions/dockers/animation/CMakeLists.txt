add_subdirectory(tests)

set(KRITA_ANIMATIONDOCKER_PART_SRCS
    animation_dockers.cpp
    animation_docker.cpp
    timeline_view.cpp
    timeline_widget.cpp
    timeline_docker.cpp
    keyframe_channel_delegate.cpp
    kis_onion_skin_dialog.cpp

    frames_item_delegate.cpp
    frames_table_view.cpp

    timeline_layers_header.cpp
    timeline_ruler_header.cpp

    timeline_frames_model.cpp

    timeline_frames_index_converter.cpp
    timeline_node_list_keeper.cpp
)

kde4_add_ui_files(KRITA_ANIMATIONDOCKER_PART_SRCS
    wdg_animation.ui
    kis_onion_skin_dialog.ui
)

kde4_add_plugin(kritaanimationdocker ${KRITA_ANIMATIONDOCKER_PART_SRCS})

target_link_libraries(kritaanimationdocker kritaui)

install(TARGETS kritaanimationdocker  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_animationdocker.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

