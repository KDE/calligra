project(kformula)

add_subdirectory( pics )

include_directories(${KFORMULA_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES})


########### next target ###############

set(kformulapart_PART_SRCS 
   KFormulaPartDocument.cpp 
   KFormulaPartView.cpp 
   KFormulaPartViewAdaptor.cpp
   KFormulaPartFactory.cpp
   KFormulaCanvas.cpp 
   KFormulaConfigDialog.cpp )

kde4_automoc(${kformulapart_PART_SRCS})

kde4_add_plugin(kformulapart WITH_PREFIX ${kformulapart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kformulapart )

target_link_libraries(kformulapart  ${KDE4_KDECORE_LIBS} kofficeui kformulalib )

install(TARGETS kformulapart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformula_KDEINIT_SRCS main.cpp )

kde4_automoc(${kformula_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kformula ${kformula_KDEINIT_SRCS})

target_link_libraries(kdeinit_kformula  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kformula  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kformula kdeinit_kformula )
install(TARGETS kformula  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  tips  DESTINATION ${DATA_INSTALL_DIR}/kformula)
install( FILES  kformulapart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kformula.rc kformula_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kformula)
install( FILES  kformula.desktop  DESTINATION ${XDG_APPS_DIR})


