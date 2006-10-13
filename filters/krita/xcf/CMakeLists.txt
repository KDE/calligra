

add_subdirectory( xcf )

include_directories( ${CMAKE_SOURCE_DIR}/krita ${CMAKE_SOURCE_DIR}/krita/core ${CMAKE_SOURCE_DIR}/krita/sdk ${CMAKE_SOURCE_DIR}/krita/core/tiles ${CMAKE_SOURCE_DIR}/krita/kritacolor ${CMAKE_SOURCE_DIR}/krita/ui ${KDE4_INCLUDES}  )


########### next target ###############

set(kritaxcfimport_PART_SRCS xcfimport.cpp )

kde4_automoc(kritaxcfimport ${kritaxcfimport_PART_SRCS})

kde4_add_plugin(kritaxcfimport WITH_PREFIX ${kritaxcfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritaxcfimport )

target_link_libraries(kritaxcfimport kritaui)

install(TARGETS kritaxcfimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kritaxcfexport_PART_SRCS xcfexport.cpp )

kde4_automoc(kritaxcfexport ${kritaxcfexport_PART_SRCS})

kde4_add_plugin(kritaxcfexport WITH_PREFIX ${kritaxcfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritaxcfexport )

target_link_libraries(kritaxcfexport kritaui)

install(TARGETS kritaxcfexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_xcf_import.desktop krita_xcf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

