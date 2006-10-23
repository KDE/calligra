




########### next target ###############

set(kfopngexport_PART_SRCS pngexport.cc pngexportdia.cc )

kde4_automoc(${kfopngexport_PART_SRCS})

kde4_add_plugin(kfopngexport WITH_PREFIX ${kfopngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfopngexport )

target_link_libraries(kfopngexport  ${KDE4_KDEUI_LIBS} kformulalib )

install(TARGETS kfopngexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


