project(quickform)

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${KDE4_INCLUDES}
)

#build a shared library
set(kexihandler_quickform_SRCS 
    quickformview.cpp 
    quickformdataview.cpp 
    quickformdesignview.cpp 
    kexiquickform.cpp
    quickformpart.cpp
    kexiscriptadapterq.cpp
    )

kde4_add_plugin(kexihandler_quickform ${kexihandler_quickform_SRCS})

target_link_libraries(kexihandler_quickform
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    kexidb
    ${CALLIGRADB_LIBS}
    kexicore
    ${KOPROPERTY_LIBS}
    kexiguiutils
)

install(TARGETS kexihandler_quickform DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kexiquickformhandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kexi)

    