include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${KOMAIN_INCLUDES}
)


if(SHOULD_BUILD_FILTER_SHEETS_TO_GNUMERIC)

set(gnumericexport_PART_SRCS gnumericexport.cc )
kde4_add_plugin(gnumericexport ${gnumericexport_PART_SRCS})
target_link_libraries(gnumericexport calligrasheetscommon kowidgets)

install(TARGETS gnumericexport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_SHEETS_TO_GNUMERIC)


if(SHOULD_BUILD_FILTER_GNUMERIC_TO_SHEETS)

set(gnumericimport_PART_SRCS gnumericimport.cc )
kde4_add_plugin(gnumericimport ${gnumericimport_PART_SRCS})
target_link_libraries(gnumericimport calligrasheetscommon kowidgets)
install(TARGETS gnumericimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_GNUMERIC_TO_SHEETS)
