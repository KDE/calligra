

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core   )


########### next target ###############

set(karbon_roundcornersplugin_PART_SRCS RoundCornersPlugin.cpp RoundCornersCommand.cpp )


add_library(karbon_roundcornersplugin MODULE ${karbon_roundcornersplugin_PART_SRCS})



target_link_libraries(karbon_roundcornersplugin karboncommon)

install(TARGETS karbon_roundcornersplugin  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  RoundCornersPlugin.rc  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/plugins)
install( FILES  karbonroundcornersplugin.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/calligra)
