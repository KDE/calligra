
include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )

########### next target ###############

set(karbon_refinepathplugin_PART_SRCS RefinePathPlugin.cpp KarbonPathRefineCommand.cpp )

kde4_add_plugin(karbon_refinepathplugin ${karbon_refinepathplugin_PART_SRCS})

target_link_libraries(karbon_refinepathplugin karboncommon)

install(TARGETS karbon_refinepathplugin  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES RefinePathPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)
