include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KDE4_INCLUDES}
    ${KOMAIN_INCLUDES}
)


set(SHOULD_BUILD_FILTER_HTML_TO_ODS FALSE)
if(SHOULD_BUILD_FILTER_HTML_TO_ODS)

set(html2ods_PART_SRCS htmlimport.cc )
kde4_add_plugin(calligra_filter_html2ods ${html2ods_PART_SRCS})
target_link_libraries(calligra_filter_html2ods komain ${KDE4_KHTML_LIBS})
install(TARGETS calligra_filter_html2ods  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_html2ods.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_HTML_TO_ODS)


if(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)

set(sheets2html_PART_SRCS htmlexport.cc exportdialog.cc )
kde4_add_ui_files(sheets2html_PART_SRCS exportwidget.ui )
kde4_add_plugin(calligra_filter_sheets2html ${sheets2html_PART_SRCS})
target_link_libraries(calligra_filter_sheets2html calligrasheetscommon)
install(TARGETS calligra_filter_sheets2html  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_sheets2html.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)
