




########### next target ###############

set(kfosvgexport_PART_SRCS svgexport.cc )


kde4_add_plugin(kfosvgexport ${kfosvgexport_PART_SRCS})



target_link_libraries(kfosvgexport  ${KDE4_KDECORE_LIBS} komain kformulalib )

install(TARGETS kfosvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


