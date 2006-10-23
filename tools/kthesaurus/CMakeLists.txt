

include_directories( ${KDE4_INCLUDES} )


########### next target ###############

set(kthesaurus_KDEINIT_SRCS main.cc )

kde4_automoc(${kthesaurus_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kthesaurus ${kthesaurus_KDEINIT_SRCS})

target_link_libraries(kdeinit_kthesaurus  ${KDE4_KIO_LIBS} )

install(TARGETS kdeinit_kthesaurus  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kthesaurus kdeinit_kthesaurus )
install(TARGETS kthesaurus  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  KThesaurus.desktop  DESTINATION ${XDG_APPS_DIR})


