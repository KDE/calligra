

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/visitors   )


########### next target ###############

set(karbonxamlexport_PART_SRCS xamlexport.cc )


add_library(karbonxamlexport MODULE ${karbonxamlexport_PART_SRCS})



target_link_libraries(karbonxamlexport    karboncommon )

install(TARGETS karbonxamlexport  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### next target ###############

set(karbonxamlimport_PART_SRCS xamlimport.cc )

add_library(karbonxamlimport MODULE ${karbonxamlimport_PART_SRCS})



target_link_libraries(karbonxamlimport    karboncommon )

install(TARGETS karbonxamlimport  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbon_xaml_export.desktop karbon_xaml_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
