

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartsvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${kchartsvgexport_PART_SRCS})

kde4_add_plugin(kchartsvgexport WITH_PREFIX ${kchartsvgexport_PART_SRCS})



target_link_libraries(kchartsvgexport  ${KDE4_KDECORE_LIBS} kchartcommon )

install(TARGETS kchartsvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



