




########### next target ###############

set(applixwordimport_PART_SRCS applixwordimport.cc )


kde4_add_plugin(applixwordimport WITH_PREFIX ${applixwordimport_PART_SRCS})



target_link_libraries(
    applixwordimport
    komain
    kstore
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS applixwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_applixword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


