include_directories( ${KDE4_INCLUDES} ${KOTEXT_INCLUDES} ${KOMAIN_INCLUDES})

########### next target ###############

set(kthesaurus_KDEINIT_SRCS main.cc )


kde4_add_kdeinit_executable( kthesaurus ${kthesaurus_KDEINIT_SRCS})

target_link_libraries(kdeinit_kthesaurus  kotext)

install(TARGETS kdeinit_kthesaurus  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kthesaurus kdeinit_kthesaurus  ${QT_QTGUI_LIBRARY}  ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS})
install(TARGETS kthesaurus  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  KThesaurus.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

