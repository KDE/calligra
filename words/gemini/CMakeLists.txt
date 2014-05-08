if (WIN32)
    option(ENABLE_GEMINI_STARTUP_MAGIC "Enable Gemini's windows startup magic" ON)

    if (ENABLE_GEMINI_STARTUP_MAGIC)
        add_definitions(-DGEMINI_ENABLE_STARTUP_MAGIC)
    endif (ENABLE_GEMINI_STARTUP_MAGIC)
endif (WIN32)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/../part
                    #${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_BINARY_DIR})

set(calligrawordsgemini_SRCS
    main.cpp
    MainWindow.cpp
    desktopviewproxy.cpp
    DocumentManager.cpp
    ProgressProxy.cpp
    QmlGlobalEngine.cpp
    RecentFileManager.cpp
    Settings.cpp
    TouchView.cpp
    TouchPart.cpp
    TouchDeclarativeView.cpp
    Theme.cpp
)


if (WIN32)
#    set(calligrawordsgemini_SRCS ${calligrawordsgemini_SRCS} KritaGeminiWin.rc)
else (WIN32)
    kde4_add_app_icon(calligrawordsgemini_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/calligrawordsgemini.png")
endif (WIN32)

# Krita Gemini application
# =============================================================================

kde4_add_executable(calligrawordsgemini ${calligrawordsgemini_SRCS})

target_link_libraries(calligrawordsgemini
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTOPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${GLEW_LIBRARIES}
    komain
    wordsprivate
)

install(TARGETS   calligrawordsgemini ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  calligrawordsgemini.desktop               DESTINATION ${XDG_APPS_INSTALL_DIR})
#install(FILES     calligrawordsgeminirc calligrawordsgeminipanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
install(FILES     calligrawordsgemini.qml                   DESTINATION ${DATA_INSTALL_DIR}/calligrawordsgemini)
#install(FILES     calligrawordsgemini.rc                    DESTINATION ${DATA_INSTALL_DIR}/calligrawordsgemini)

#add_subdirectory(icon)
