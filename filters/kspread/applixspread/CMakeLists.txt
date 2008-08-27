




########### next target ###############

set(applixspreadimport_PART_SRCS applixspreadimport.cc )


kde4_add_plugin(applixspreadimport WITH_PREFIX ${applixspreadimport_PART_SRCS})



target_link_libraries(applixspreadimport ${QT_QTGUI_LIBRARY} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDECORE_LIBS} komain )

install(TARGETS applixspreadimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_applixspread_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})




