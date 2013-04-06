include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KDE4_INCLUDES}
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_HTML_TO_ODS)

set(kspreadhtmlimport_PART_SRCS htmlimport.cc )
kde4_add_plugin(kspreadhtmlimport ${kspreadhtmlimport_PART_SRCS})
target_link_libraries(kspreadhtmlimport komain ${KDE4_KHTML_LIBS})
install(TARGETS kspreadhtmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kspread_html_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_HTML_TO_ODS)


if(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)

set(kspreadhtmlexport_PART_SRCS htmlexport.cc exportdialog.cc )
kde4_add_ui_files(kspreadhtmlexport_PART_SRCS exportwidget.ui )
kde4_add_plugin(kspreadhtmlexport ${kspreadhtmlexport_PART_SRCS})
target_link_libraries(kspreadhtmlexport calligrasheetscommon)
install(TARGETS kspreadhtmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kspread_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)
