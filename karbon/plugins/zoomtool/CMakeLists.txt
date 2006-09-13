

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_zoomtoolplugin_PART_SRCS vzoomtool.cc zoomtoolplugin.cc )

kde4_automoc(${karbon_zoomtoolplugin_PART_SRCS})

kde4_add_plugin(karbon_zoomtoolplugin ${karbon_zoomtoolplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_zoomtoolplugin )

target_link_libraries(karbon_zoomtoolplugin  ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install(TARGETS karbon_zoomtoolplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbonzoomtool.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

