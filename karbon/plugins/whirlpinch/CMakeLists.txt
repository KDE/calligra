
include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )

########### next target ###############

set(karbon_whirlpinchplugin_PART_SRCS WhirlPinchPlugin.cpp KarbonWhirlPinchCommand.cpp )

kde4_add_plugin(karbon_whirlpinchplugin ${karbon_whirlpinchplugin_PART_SRCS})

target_link_libraries(karbon_whirlpinchplugin karboncommon)

install(TARGETS karbon_whirlpinchplugin  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  WhirlPinchPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)

