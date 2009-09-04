

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_roundcornersplugin_PART_SRCS RoundCornersPlugin.cpp RoundCornersCommand.cpp )


kde4_add_plugin(karbon_roundcornersplugin ${karbon_roundcornersplugin_PART_SRCS})



target_link_libraries(karbon_roundcornersplugin karboncommon)

install(TARGETS karbon_roundcornersplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  RoundCornersPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)





