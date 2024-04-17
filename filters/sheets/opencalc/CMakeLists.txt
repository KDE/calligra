include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_OPENCALC_TO_SHEETS)

set(opencalc2sheets_PART_SRCS opencalcimport.cc ooutils.cc )

add_library(calligra_filter_opencalc2sheets MODULE ${opencalc2sheets_PART_SRCS})

target_link_libraries(calligra_filter_opencalc2sheets calligrasheetsui kowidgets KF6::Archive)

install(TARGETS calligra_filter_opencalc2sheets  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()


if(SHOULD_BUILD_FILTER_SHEETS_TO_OPENCALC)

set(sheets2opencalc_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

add_library(calligra_filter_sheets2opencalc MODULE ${sheets2opencalc_PART_SRCS})

target_link_libraries(calligra_filter_sheets2opencalc calligrasheetsui calligrasheetspartlib )

install(TARGETS calligra_filter_sheets2opencalc  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

endif()
