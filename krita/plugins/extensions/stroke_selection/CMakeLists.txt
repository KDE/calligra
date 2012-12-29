set(kritastrokeselection_PART_SRCS
    dlg_stroke_selection.cpp
    stroke_selection.cpp
)


kde4_add_ui_files(kritastrokeselection_PART_SRCS
    wdg_stroke_selection.ui
)

kde4_add_plugin(kritastrokeselection ${kritastrokeselection_PART_SRCS})

target_link_libraries(kritastrokeselection kritaui)

install(TARGETS kritastrokeselection  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  stroke_selection.rc  DESTINATION ${DATA_INSTALL_DIR}/kritaplugins)
install( FILES  kritastrokeselection.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

