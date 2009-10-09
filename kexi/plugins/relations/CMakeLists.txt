
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/relations ${CMAKE_SOURCE_DIR}/kexi/tableview ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_relation_PART_SRCS kexirelationpartimpl.cpp kexirelationmaindlg.cpp )


kde4_add_plugin(kexihandler_relation ${kexihandler_relation_PART_SRCS})



target_link_libraries(
    kexihandler_relation
    kexicore
    kexirelationsview
    kexidb
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kexihandler_relation  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexirelationhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
