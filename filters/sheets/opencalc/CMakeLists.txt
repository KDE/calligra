include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_OPENCALC_TO_SHEETS)

set(opencalc2sheets_PART_SRCS opencalcimport.cc ooutils.cc )

add_library(calligra_filter_opencalc2sheets MODULE ${opencalc2sheets_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_opencalc2sheets calligra_filter_opencalc2sheets.desktop)

target_link_libraries(calligra_filter_opencalc2sheets calligrasheetscommon kowidgets KF5::Archive)

install(TARGETS calligra_filter_opencalc2sheets  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

endif()


if(SHOULD_BUILD_FILTER_SHEETS_TO_OPENCALC)

set(sheets2opencalc_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

add_library(calligra_filter_sheets2opencalc MODULE ${sheets2opencalc_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_sheets2opencalc calligra_filter_sheets2opencalc.desktop)

target_link_libraries(calligra_filter_sheets2opencalc calligrasheetscommon)

install(TARGETS calligra_filter_sheets2opencalc  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

endif()
