
include_directories( ${CMAKE_SOURCE_DIR}/karbon )

########### next target ###############

set(karbon_whirlpinch_PART_SRCS WhirlPinchPlugin.cpp KarbonWhirlPinchCommand.cpp )

add_library(karbon_whirlpinch MODULE ${karbon_whirlpinch_PART_SRCS})

karbon_viewplugin_desktop_to_json(karbon_whirlpinch karbon_whirlpinch.desktop)

target_link_libraries(karbon_whirlpinch karboncommon)

install(TARGETS karbon_whirlpinch  DESTINATION ${PLUGIN_INSTALL_DIR}/karbon/extensions)

########### install files ###############

install( FILES  WhirlPinchPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/plugins)
