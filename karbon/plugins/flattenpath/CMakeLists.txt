include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )


########### next target ###############

set(karbon_flattenpathplugin_PART_SRCS FlattenPathPlugin.cpp KarbonPathFlattenCommand.cpp )


add_library(karbon_flattenpathplugin MODULE ${karbon_flattenpathplugin_PART_SRCS})



target_link_libraries(karbon_flattenpathplugin karboncommon)

install(TARGETS karbon_flattenpathplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  FlattenPathPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/plugins)
install( FILES  karbonflattenpathplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)



