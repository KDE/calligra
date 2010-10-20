include_directories( ${KDE4_INCLUDES} ${KOTEXT_INCLUDES} ${KOMAIN_INCLUDES})

########### next target ###############

set(kthesaurus_KDEINIT_SRCS main.cc )


kde4_add_kdeinit_executable( kthesaurus ${kthesaurus_KDEINIT_SRCS})

target_link_libraries(kdeinit_kthesaurus  kotext ${KDE4_KDEUI_LIBS})

install(TARGETS kdeinit_kthesaurus  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( kthesaurus kdeinit_kthesaurus)
install(TARGETS kthesaurus  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS  KThesaurus.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

