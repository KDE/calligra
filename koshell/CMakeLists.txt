
project(koshell)
include_directories(${KOFFICEUI_INCLUDES})


########### next target ###############

set(koshell_KDEINIT_SRCS koshell_main.cc koshell_shell.cc iconsidepane.cpp )

kde4_automoc(${koshell_KDEINIT_SRCS})

kde4_add_kcfg_files(koshell_KDEINIT_SRCS koshellsettings.kcfgc )

kde4_add_kdeinit_executable( koshell ${koshell_KDEINIT_SRCS})

target_link_libraries(kdeinit_koshell  ${KDE4_KDECORE_LIBS} kofficeui )

install(TARGETS kdeinit_koshell  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( koshell kdeinit_koshell )
install(TARGETS koshell  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  koshell.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  koshell.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES  koshellui.rc  DESTINATION ${DATA_INSTALL_DIR}/koshell)

kde4_install_icons( ${ICON_INSTALL_DIR}   )


