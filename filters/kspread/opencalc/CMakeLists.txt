include_directories( ${CMAKE_SOURCE_DIR}/kspread ${CMAKE_SOURCE_DIR}/filters/liboofilter ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )


########### next target ###############

set(opencalcimport_PART_SRCS opencalcimport.cc ${liboofilter_SRCS} )


kde4_add_plugin(opencalcimport WITH_PREFIX ${opencalcimport_PART_SRCS})



target_link_libraries(opencalcimport kspreadcommon kopagelayout)

install(TARGETS opencalcimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(opencalcexport_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

kde4_add_plugin(opencalcexport WITH_PREFIX ${opencalcexport_PART_SRCS})



target_link_libraries(opencalcexport kspreadcommon)

install(TARGETS opencalcexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_opencalc_import.desktop kspread_opencalc_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



