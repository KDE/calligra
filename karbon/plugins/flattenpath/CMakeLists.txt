include_directories( ${CMAKE_SOURCE_DIR}/karbon )


########### next target ###############

set(karbon_flattenpath_PART_SRCS FlattenPathPlugin.cpp KarbonPathFlattenCommand.cpp )

add_library(karbon_flattenpath MODULE ${karbon_flattenpath_PART_SRCS})

target_link_libraries(karbon_flattenpath karboncommon)

install(TARGETS karbon_flattenpath  DESTINATION ${KDE_INSTALL_PLUGINDIR}/karbon/extensions)


########### install files ###############

install( FILES  FlattenPathPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
