include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${KDE4_INCLUDES}
    ${KOKROSS_INCLUDES}
)

########### next target ###############

set(kexihandler_script_PART_SRCS kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp )
qt4_wrap_cpp(kexihandler_script_PART_SRCS kexiscriptadaptor.h)
kde4_add_plugin(kexihandler_script ${kexihandler_script_PART_SRCS})
target_link_libraries(
    kexihandler_script
    kexiextendedwidgets
    ${KOPROPERTY_LIBS}
    kokross
    kexicore
    kexiutils
    kexidb
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )
install(TARGETS kexihandler_script  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kexiscripthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
