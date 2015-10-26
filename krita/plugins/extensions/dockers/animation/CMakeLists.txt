add_subdirectory(tests)

set(KRITA_ANIMATIONDOCKER_PART_SRCS
    animation_dockers.cpp
    animation_docker.cpp
    timeline_docker.cpp
    kis_onion_skin_dialog.cpp

    timeline_layers_header.cpp
    timeline_ruler_header.cpp

    timeline_frames_model.cpp
    timeline_frames_view.cpp
    timeline_frames_item_delegate.cpp

    timeline_frames_index_converter.cpp
    timeline_node_list_keeper.cpp

    kis_draggable_tool_button.cpp
    kis_animation_utils.cpp
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

