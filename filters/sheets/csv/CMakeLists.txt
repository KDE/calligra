include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)

if(SHOULD_BUILD_FILTER_CSV_TO_SHEETS)

set(csv2sheets_PART_SRCS csvimport.cc)

add_library(calligra_filter_csv2sheets MODULE ${csv2sheets_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_csv2sheets calligra_filter_csv2sheets.desktop)

target_link_libraries(calligra_filter_csv2sheets calligrasheetscommon kowidgets)

install(TARGETS calligra_filter_csv2sheets DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

endif()


if(SHOULD_BUILD_FILTER_SHEETS_TO_CSV)

set(sheets2csv_PART_SRCS csvexport.cc csvexportdialog.cpp )

ki18n_wrap_ui(sheets2csv_PART_SRCS exportdialogui.ui )

add_library(calligra_filter_sheets2csv MODULE ${sheets2csv_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_sheets2csv calligra_filter_sheets2csv.desktop)

target_link_libraries(calligra_filter_sheets2csv calligrasheetscommon)

install(TARGETS calligra_filter_sheets2csv  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

endif()
