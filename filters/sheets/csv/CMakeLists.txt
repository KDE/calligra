include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)

if(SHOULD_BUILD_FILTER_CSV_TO_SHEETS)

set(csvimport_PART_SRCS csvimport.cc)

kde4_add_plugin(csvimport ${csvimport_PART_SRCS})

target_link_libraries(csvimport calligrasheetscommon kowidgets)

install(TARGETS csvimport DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_csv_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_CSV_TO_SHEETS)


if(SHOULD_BUILD_FILTER_SHEETS_TO_CSV)

set(csvexport_PART_SRCS csvexport.cc csvexportdialog.cpp )

kde4_add_ui_files(csvexport_PART_SRCS exportdialogui.ui )

kde4_add_plugin(csvexport ${csvexport_PART_SRCS})

target_link_libraries(csvexport calligrasheetscommon)

install(TARGETS csvexport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_csv_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_CSV)
