
include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )

########### next target ###############

set(karbon_whirlpinch_PART_SRCS WhirlPinchPlugin.cpp KarbonWhirlPinchCommand.cpp )

add_library(karbon_whirlpinch MODULE ${karbon_whirlpinch_PART_SRCS})

kcoreaddons_desktop_to_json(karbon_whirlpinch karbon_whirlpinch.desktop)

target_link_libraries(karbon_whirlpinch karboncommon)

install(TARGETS karbon_whirlpinch  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  WhirlPinchPlugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/plugins)
