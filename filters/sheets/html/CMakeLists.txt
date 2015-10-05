include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_HTML_TO_ODS)

set(html2ods_PART_SRCS htmlimport.cc )
add_library(calligra_filter_html2ods MODULE ${html2ods_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_html2ods calligra_filter_html2ods.desktop)
target_link_libraries(calligra_filter_html2ods komain KF5::KHtml KF5::KDELibs4Support)
install(TARGETS calligra_filter_html2ods  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

endif()


if(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)

set(sheets2html_PART_SRCS htmlexport.cc exportdialog.cc )
ki18n_wrap_ui(sheets2html_PART_SRCS exportwidget.ui )
add_library(calligra_filter_sheets2html MODULE ${sheets2html_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_sheets2html calligra_filter_sheets2html.desktop)
target_link_libraries(calligra_filter_sheets2html calligrasheetscommon KF5::KIOWidgets)
install(TARGETS calligra_filter_sheets2html  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

endif()
