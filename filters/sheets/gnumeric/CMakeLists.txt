include_directories( ${CMAKE_SOURCE_DIR}/sheets ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )

set(gnumericexport_PART_SRCS gnumericexport.cc )
kde4_add_plugin(gnumericexport ${gnumericexport_PART_SRCS})
target_link_libraries(gnumericexport calligrasheetscommon kowidgets)

install(TARGETS gnumericexport  DESTINATION ${PLUGIN_INSTALL_DIR})

set(gnumericimport_PART_SRCS gnumericimport.cc )
kde4_add_plugin(gnumericimport ${gnumericimport_PART_SRCS})
target_link_libraries(gnumericimport calligrasheetscommon kowidgets)
install(TARGETS gnumericimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

