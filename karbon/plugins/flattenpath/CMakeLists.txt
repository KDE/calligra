

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_flattenpathplugin_PART_SRCS flattenpathplugin.cc )

kde4_automoc(${karbon_flattenpathplugin_PART_SRCS})

kde4_add_plugin(karbon_flattenpathplugin ${karbon_flattenpathplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_flattenpathplugin )

target_link_libraries(karbon_flattenpathplugin   ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install(TARGETS karbon_flattenpathplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  flattenpathplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)


