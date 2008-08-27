




########### next target ###############

set(wpexport_PART_SRCS wpexport.cc wp5.cc wp6.cc )


kde4_add_plugin(wpexport WITH_PREFIX ${wpexport_PART_SRCS})



target_link_libraries(
    wpexport
    kwordexportfilters
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS wpexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wp_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


