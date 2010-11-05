




########### next target ###############

set(kfolatexexport_PART_SRCS latexexport.cc )


kde4_add_plugin(kfolatexexport ${kfolatexexport_PART_SRCS})



target_link_libraries(kfolatexexport  ${KDE4_KDECORE_LIBS} komain kformulalib )

install(TARGETS kfolatexexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_latex_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

