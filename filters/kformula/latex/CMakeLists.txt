




########### next target ###############

set(kfolatexexport_PART_SRCS latexexport.cc )

kde4_automoc(${kfolatexexport_PART_SRCS})

kde4_add_plugin(kfolatexexport WITH_PREFIX ${kfolatexexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfolatexexport )

target_link_libraries(kfolatexexport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install(TARGETS kfolatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

