include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_HTML_TO_ODS)

set(html2ods_PART_SRCS htmlimport.cc HtmlImportDebug.cpp)
add_library(calligra_filter_html2ods MODULE ${html2ods_PART_SRCS})
target_link_libraries(calligra_filter_html2ods komain)
install(TARGETS calligra_filter_html2ods  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()


if(SHOULD_BUILD_FILTER_SHEETS_TO_HTML)

set(sheets2html_PART_SRCS htmlexport.cc exportdialog.cc )
ki18n_wrap_ui(sheets2html_PART_SRCS exportwidget.ui )
add_library(calligra_filter_sheets2html MODULE ${sheets2html_PART_SRCS})
target_link_libraries(calligra_filter_sheets2html calligrasheetsui KF6::KIOWidgets)
install(TARGETS calligra_filter_sheets2html  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()
