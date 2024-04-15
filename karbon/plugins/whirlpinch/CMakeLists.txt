
include_directories( ${CMAKE_SOURCE_DIR}/karbon )

########### next target ###############

set(karbon_whirlpinch_PART_SRCS WhirlPinchPlugin.cpp KarbonWhirlPinchCommand.cpp )

add_library(karbon_whirlpinch MODULE ${karbon_whirlpinch_PART_SRCS})

target_link_libraries(karbon_whirlpinch karboncommon)

install(TARGETS karbon_whirlpinch  DESTINATION ${KDE_INSTALL_PLUGINDIR}/karbon/extensions)

########### install files ###############

install( FILES  WhirlPinchPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
