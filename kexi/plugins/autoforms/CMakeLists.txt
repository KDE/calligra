include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${KDE4_INCLUDES}
)

#build a shared library
set(kexihandler_autoform_SRCS
    KexiAutoForms.cpp
    KexiAutoFormPart.cpp
    KexiAutoFormView.cpp
    widgets/AutoWidget.cpp
    KexiAutoFormDesignView.cpp
    designer/KexiAutoFormDesigner.cpp 
)

kde4_add_plugin(kexihandler_autoform ${kexihandler_autoform_SRCS})

target_link_libraries(kexihandler_autoform
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KPARTS_LIBS}
    kexidb
    kexicore
    kexiextendedwidgets
    kexiguiutils
)

install(TARGETS kexihandler_autoform DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kexiautoformhandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
