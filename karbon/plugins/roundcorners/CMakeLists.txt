

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_roundcornersplugin_PART_SRCS roundcornersplugin.cc )

kde4_automoc(${karbon_roundcornersplugin_PART_SRCS})

kde4_add_plugin(karbon_roundcornersplugin ${karbon_roundcornersplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_roundcornersplugin )

target_link_libraries(karbon_roundcornersplugin   ${KDE4_KPARTS_LIBS} karboncommon )

install(TARGETS karbon_roundcornersplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  roundcornersplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)





