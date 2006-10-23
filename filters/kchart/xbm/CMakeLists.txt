

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartxbmexport_PART_SRCS xbmexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartxbmexport_PART_SRCS})

kde4_add_plugin(kchartxbmexport WITH_PREFIX ${kchartxbmexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartxbmexport )

target_link_libraries(kchartxbmexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartxbmexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_xbm_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

