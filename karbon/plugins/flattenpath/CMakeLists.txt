

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_flattenpathplugin_PART_SRCS FlattenPathPlugin.cpp KarbonPathFlattenCommand.cpp )


kde4_add_plugin(karbon_flattenpathplugin ${karbon_flattenpathplugin_PART_SRCS})



target_link_libraries(karbon_flattenpathplugin karboncommon)

install(TARGETS karbon_flattenpathplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  FlattenPathPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)


