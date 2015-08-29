

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )


########### next target ###############

set(karbon_roundcorners_PART_SRCS RoundCornersPlugin.cpp RoundCornersCommand.cpp )


add_library(karbon_roundcorners MODULE ${karbon_roundcorners_PART_SRCS})

kcoreaddons_desktop_to_json(karbon_roundcorners karbon_roundcorners.desktop)

target_link_libraries(karbon_roundcorners karboncommon)

install(TARGETS karbon_roundcorners  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  RoundCornersPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
