
include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )

########### next target ###############

set(karbon_refinepathplugin_PART_SRCS RefinePathPlugin.cpp KarbonPathRefineCommand.cpp )

add_library(karbon_refinepathplugin MODULE ${karbon_refinepathplugin_PART_SRCS})

target_link_libraries(karbon_refinepathplugin karboncommon)

install(TARGETS karbon_refinepathplugin  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES RefinePathPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/plugins)
install( FILES karbonrefinepathplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

