include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(gnumericexport_PART_SRCS gnumericexport.cc )
kde4_add_plugin(gnumericexport WITH_PREFIX ${gnumericexport_PART_SRCS})
target_link_libraries(gnumericexport kspreadcommon kowidgets)

install(TARGETS gnumericexport  DESTINATION ${PLUGIN_INSTALL_DIR})

set(gnumericimport_PART_SRCS gnumericimport.cc )
kde4_add_plugin(gnumericimport WITH_PREFIX ${gnumericimport_PART_SRCS})
target_link_libraries(gnumericimport kspreadcommon kowidgets)
install(TARGETS gnumericimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

