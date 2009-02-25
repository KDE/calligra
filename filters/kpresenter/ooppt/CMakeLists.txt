set(oopptimport_PART_SRCS oopptimport.cc)

kde4_add_plugin(oopptimport WITH_PREFIX ${oopptimport_PART_SRCS})

target_link_libraries(
    oopptimport
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS oopptimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kpresenter_ooppt_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS KOfficeOODocumentConverter.py DESTINATION ${BIN_INSTALL_DIR})
