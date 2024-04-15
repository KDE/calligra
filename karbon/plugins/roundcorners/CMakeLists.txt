

include_directories( ${CMAKE_SOURCE_DIR}/karbon )


########### next target ###############

set(karbon_roundcorners_PART_SRCS RoundCornersPlugin.cpp RoundCornersCommand.cpp )

add_library(karbon_roundcorners MODULE ${karbon_roundcorners_PART_SRCS})

target_link_libraries(karbon_roundcorners karboncommon)

install(TARGETS karbon_roundcorners  DESTINATION ${KDE_INSTALL_PLUGINDIR}/karbon/extensions)


########### install files ###############

install( FILES  RoundCornersPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
