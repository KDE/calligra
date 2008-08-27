




########### next target ###############

set(rtfimport_PART_SRCS rtfimport.cpp rtfimport_dom.cpp rtfimport_tokenizer.cpp )


kde4_add_plugin(rtfimport WITH_PREFIX ${rtfimport_PART_SRCS})



target_link_libraries(
    rtfimport
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS rtfimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_rtf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
