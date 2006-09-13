

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_insertknotsplugin_PART_SRCS insertknotsplugin.cc )

kde4_automoc(${karbon_insertknotsplugin_PART_SRCS})

kde4_add_plugin(karbon_insertknotsplugin ${karbon_insertknotsplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_insertknotsplugin )

target_link_libraries(karbon_insertknotsplugin  ${KDE4_KPARTS_LIBS} karboncommon )

install(TARGETS karbon_insertknotsplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  insertknotsplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)

