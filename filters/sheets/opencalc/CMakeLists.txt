include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/filters/liboofilter
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_OPENCALC_TO_SHEETS)

set(opencalc2sheets_PART_SRCS opencalcimport.cc ${liboofilter_SRCS} )

kde4_add_plugin(calligra_filter_opencalc2sheets ${opencalc2sheets_PART_SRCS})

target_link_libraries(calligra_filter_opencalc2sheets calligrasheetscommon kowidgets)

install(TARGETS calligra_filter_opencalc2sheets  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_opencalc_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_OPENCALC_TO_SHEETS)


if(SHOULD_BUILD_FILTER_SHEETS_TO_OPENCALC)

set(sheets2opencalc_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

kde4_add_plugin(calligra_filter_sheets2opencalc ${sheets2opencalc_PART_SRCS})

target_link_libraries(calligra_filter_sheets2opencalc calligrasheetscommon)

install(TARGETS calligra_filter_sheets2opencalc  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_opencalc_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_OPENCALC)
