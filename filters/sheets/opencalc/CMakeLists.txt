include_directories( ${CMAKE_SOURCE_DIR}/sheets ${CMAKE_SOURCE_DIR}/filters/liboofilter ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )


########### next target ###############

set(opencalcimport_PART_SRCS opencalcimport.cc ${liboofilter_SRCS} )


kde4_add_plugin(opencalcimport ${opencalcimport_PART_SRCS})



target_link_libraries(opencalcimport calligrasheetscommon kowidgets)

install(TARGETS opencalcimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(opencalcexport_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

kde4_add_plugin(opencalcexport ${opencalcexport_PART_SRCS})



target_link_libraries(opencalcexport calligrasheetscommon)

install(TARGETS opencalcexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_opencalc_import.desktop kspread_opencalc_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



