




########### next target ###############

set( kformula13import_PART_SRCS KFormula13Import.cpp
                                KFormula13ContentHandler.cpp )

kde4_automoc(${kformula13import_PART_SRCS})

kde4_add_plugin(kformula13import WITH_PREFIX ${kformula13import_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kformula13import )

target_link_libraries(kformula13import  ${KDE4_KDECORE_LIBS} kofficecore kformulalib )

install(TARGETS kformula13import  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformula_kformula13_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

