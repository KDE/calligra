include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_SHEETS_TO_GNUMERIC)

set(sheets2gnumeric_PART_SRCS gnumericexport.cc )
kde4_add_plugin(calligra_filter_sheets2gnumeric ${sheets2gnumeric_PART_SRCS})
target_link_libraries(calligra_filter_sheets2gnumeric calligrasheetscommon kowidgets)

install(TARGETS calligra_filter_sheets2gnumeric  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_GNUMERIC)


if(SHOULD_BUILD_FILTER_GNUMERIC_TO_SHEETS)

set(gnumeric2sheets_PART_SRCS gnumericimport.cc )
kde4_add_plugin(calligra_filter_gnumeric2sheets ${gnumeric2sheets_PART_SRCS})
target_link_libraries(calligra_filter_gnumeric2sheets calligrasheetscommon kowidgets)
install(TARGETS calligra_filter_gnumeric2sheets  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_GNUMERIC_TO_SHEETS)
