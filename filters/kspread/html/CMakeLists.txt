

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadhtmlexport_PART_SRCS htmlexport.cc exportdialog.cc )


kde4_add_ui_files(kspreadhtmlexport_PART_SRCS exportwidget.ui )

kde4_add_plugin(kspreadhtmlexport WITH_PREFIX ${kspreadhtmlexport_PART_SRCS})



target_link_libraries(kspreadhtmlexport ${KDE4_KIO_LIBS} ${QT_QT3SUPPORT_LIBRARY} komain  ${KDE4_KDEUI_LIBS} kspreadcommon )

install(TARGETS kspreadhtmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


