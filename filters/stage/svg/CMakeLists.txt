

include_directories( ${CMAKE_SOURCE_DIR}/stage ${CMAKE_SOURCE_DIR}/lib/kotext ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresentersvgexport_PART_SRCS svgexport.cc )


kde4_add_plugin(kpresentersvgexport ${kpresentersvgexport_PART_SRCS})



target_link_libraries(kpresentersvgexport  ${KDE4_KDECORE_LIBS} calligrastageprivate )

install(TARGETS kpresentersvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


