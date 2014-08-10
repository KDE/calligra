

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonxamlexport_PART_SRCS xamlexport.cc )


kde4_add_plugin(karbonxamlexport ${karbonxamlexport_PART_SRCS})



target_link_libraries(karbonxamlexport  ${KDE4_KDECORE_LIBS}  karboncommon )

install(TARGETS karbonxamlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(karbonxamlimport_PART_SRCS xamlimport.cc )

kde4_add_plugin(karbonxamlimport ${karbonxamlimport_PART_SRCS})



target_link_libraries(karbonxamlimport  ${KDE4_KDECORE_LIBS}  karboncommon )

install(TARGETS karbonxamlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbon_xaml_export.desktop karbon_xaml_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
