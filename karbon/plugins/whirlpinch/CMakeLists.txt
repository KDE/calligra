

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_whirlpinchplugin_PART_SRCS whirlpinchplugin.cc )

kde4_automoc(${karbon_whirlpinchplugin_PART_SRCS})

kde4_add_plugin(karbon_whirlpinchplugin ${karbon_whirlpinchplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_whirlpinchplugin )

target_link_libraries(karbon_whirlpinchplugin  ${KDE4_KPARTS_LIBS} kopainter karboncommon )

install(TARGETS karbon_whirlpinchplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  whirlpinchplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)

