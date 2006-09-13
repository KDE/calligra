

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_shadoweffectplugin_PART_SRCS shadoweffectplugin.cc vshadowdecorator.cc )

kde4_automoc(${karbon_shadoweffectplugin_PART_SRCS})

kde4_add_plugin(karbon_shadoweffectplugin ${karbon_shadoweffectplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_shadoweffectplugin )

target_link_libraries(karbon_shadoweffectplugin   ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install(TARGETS karbon_shadoweffectplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  shadoweffectplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)

