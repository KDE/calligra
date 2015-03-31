include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )


########### next target ###############

set(karbon_flattenpathplugin_PART_SRCS FlattenPathPlugin.cpp KarbonPathFlattenCommand.cpp )


add_library(karbon_flattenpathplugin MODULE ${karbon_flattenpathplugin_PART_SRCS})



target_link_libraries(karbon_flattenpathplugin karboncommon)

install(TARGETS karbon_flattenpathplugin  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  FlattenPathPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
install( FILES  karbonflattenpathplugin.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/calligra)



