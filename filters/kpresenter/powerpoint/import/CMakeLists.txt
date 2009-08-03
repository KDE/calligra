set(powerpointimport_PART_SRCS powerpointimport.cc ${libppt_SRCS} )

kde4_add_plugin(powerpointimport WITH_PREFIX ${powerpointimport_PART_SRCS})

target_link_libraries(
    powerpointimport
    komain
    kostore
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
)

install(TARGETS powerpointimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kpresenter_powerpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

