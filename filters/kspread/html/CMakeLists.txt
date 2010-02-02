include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )

set(kspreadhtmlimport_PART_SRCS htmlimport.cc )
kde4_add_plugin(kspreadhtmlimport WITH_PREFIX ${kspreadhtmlimport_PART_SRCS})
target_link_libraries(kspreadhtmlimport komain ${KDE4_KHTML_LIBS})
install(TARGETS kspreadhtmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kspread_html_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

set(kspreadhtmlexport_PART_SRCS htmlexport.cc exportdialog.cc )
kde4_add_ui_files(kspreadhtmlexport_PART_SRCS exportwidget.ui )
kde4_add_plugin(kspreadhtmlexport WITH_PREFIX ${kspreadhtmlexport_PART_SRCS})
target_link_libraries(kspreadhtmlexport kspreadcommon)
install(TARGETS kspreadhtmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kspread_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

