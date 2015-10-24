include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )


########### next target ###############

set(karbon_flattenpath_PART_SRCS FlattenPathPlugin.cpp KarbonPathFlattenCommand.cpp )


add_library(karbon_flattenpath MODULE ${karbon_flattenpath_PART_SRCS})

kcoreaddons_desktop_to_json(karbon_flattenpath karbon_flattenpath.desktop)

target_link_libraries(karbon_flattenpath karboncommon)

install(TARGETS karbon_flattenpath  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  FlattenPathPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/plugins)
