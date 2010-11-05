




########### next target ###############

set(kfopngexport_PART_SRCS pngexport.cc pngexportdia.cc )


kde4_add_plugin(kfopngexport ${kfopngexport_PART_SRCS})



target_link_libraries(kfopngexport  ${KDE4_KDEUI_LIBS} kformulalib )

install(TARGETS kfopngexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


