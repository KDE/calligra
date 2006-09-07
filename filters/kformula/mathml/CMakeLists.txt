




########### next target ###############

set(kfomathmlexport_PART_SRCS mathmlexport.cc )

kde4_automoc(${kfomathmlexport_PART_SRCS})

kde4_add_plugin(kfomathmlexport WITH_PREFIX ${kfomathmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfomathmlexport )

target_link_libraries(kfomathmlexport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib)

install(TARGETS kfomathmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kfomathmlimport_PART_SRCS mathmlimport.cc )

kde4_automoc(${kfomathmlimport_PART_SRCS})

kde4_add_plugin(kfomathmlimport WITH_PREFIX ${kfomathmlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfomathmlimport )

target_link_libraries(kfomathmlimport  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install(TARGETS kfomathmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_mathml_export.desktop kformula_mathml_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

