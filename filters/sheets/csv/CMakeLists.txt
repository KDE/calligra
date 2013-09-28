include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)

if(SHOULD_BUILD_FILTER_CSV_TO_SHEETS)

set(csv2sheets_PART_SRCS csvimport.cc)

kde4_add_plugin(calligra_filter_csv2sheets ${csv2sheets_PART_SRCS})

target_link_libraries(calligra_filter_csv2sheets calligrasheetscommon kowidgets)

install(TARGETS calligra_filter_csv2sheets DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_csv2sheets.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_CSV_TO_SHEETS)


if(SHOULD_BUILD_FILTER_SHEETS_TO_CSV)

set(sheets2csv_PART_SRCS csvexport.cc csvexportdialog.cpp )

kde4_add_ui_files(sheets2csv_PART_SRCS exportdialogui.ui )

kde4_add_plugin(calligra_filter_sheets2csv ${sheets2csv_PART_SRCS})

target_link_libraries(calligra_filter_sheets2csv calligrasheetscommon)

install(TARGETS calligra_filter_sheets2csv  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_sheets2csv.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_CSV)
