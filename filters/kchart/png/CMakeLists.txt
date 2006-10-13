

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartpngexport_PART_SRCS pngexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(kchartpngexport ${kchartpngexport_PART_SRCS})

kde4_add_plugin(kchartpngexport WITH_PREFIX ${kchartpngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartpngexport )

target_link_libraries(kchartpngexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartpngexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

