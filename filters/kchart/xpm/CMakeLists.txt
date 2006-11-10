

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartxpmexport_PART_SRCS xpmexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartxpmexport_PART_SRCS})

kde4_add_plugin(kchartxpmexport WITH_PREFIX ${kchartxpmexport_PART_SRCS})



target_link_libraries(kchartxpmexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartxpmexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_xpm_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

