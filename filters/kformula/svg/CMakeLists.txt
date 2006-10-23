




########### next target ###############

set(kfosvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${kfosvgexport_PART_SRCS})

kde4_add_plugin(kfosvgexport WITH_PREFIX ${kfosvgexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfosvgexport )

target_link_libraries(kfosvgexport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install(TARGETS kfosvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


