
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget
 ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/kexidb ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )

########### next target ###############

set(kexihandler_table_PART_SRCS kexitablepart.cpp kexitabledesignerview.cpp
  kexitabledesigner_dataview.cpp kexitabledesignercommands.cpp
  kexitabledesignerview_p.cpp kexilookupcolumnpage.cpp )


kde4_add_plugin(kexihandler_table ${kexihandler_table_PART_SRCS})



target_link_libraries(
    kexihandler_table
    kexicore
    kexidb
    ${CALLIGRADB_LIBS}
    kexidataviewcommon
    kexidatatable
    kexiextendedwidgets
    ${KOPROPERTY_LIBS}
    kexiutils
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kexihandler_table  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexitablehandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
