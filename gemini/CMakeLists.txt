project(calligragemini)

if (WIN32)
    option(ENABLE_GEMINI_STARTUP_MAGIC "Enable Gemini's windows startup magic" ON)

    if (ENABLE_GEMINI_STARTUP_MAGIC)
        add_definitions(-DGEMINI_ENABLE_STARTUP_MAGIC)
    endif (ENABLE_GEMINI_STARTUP_MAGIC)
endif (WIN32)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KOMAIN_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/../part
                    #${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/libs/kopageapp
                    ${CMAKE_SOURCE_DIR}/stage
                    ${CMAKE_SOURCE_DIR}/words
                    ${CMAKE_SOURCE_DIR}/words/part)

set(calligragemini_SRCS
    main.cpp
    MainWindow.cpp
    desktopviewproxy.cpp
    Constants.cpp
    DocumentListModel.cpp
    DocumentManager.cpp
    ProgressProxy.cpp
    QmlGlobalEngine.cpp
    RecentFileManager.cpp
    Settings.cpp
    SimpleTouchArea.cpp
    TouchDeclarativeView.cpp
    Theme.cpp
    ToolManager.cpp
    ParagraphStylesModel.cpp
    KeyboardModel.cpp
    ScribbleArea.cpp
    RecentImageImageProvider.cpp
    RecentFilesModel.cpp
)

set(thumbnailhelper_SRCS
    thumbnailhelper.cpp
    ThumbnailHelperImpl.cpp
)

if (WIN32)
    set(calligragemini_SRCS ${calligragemini_SRCS} calligrageminiwin.rc)
else (WIN32)
    kde4_add_app_icon(calligragemini_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/calligragemini.png")
endif (WIN32)

# Thumbnail helper application
# =============================================================================

kde4_add_executable(calligrageminithumbnailhelper ${thumbnailhelper_SRCS})
target_link_libraries(calligrageminithumbnailhelper
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_LIBRARIES}
    komain
)

# Calligra Gemini application
# =============================================================================

kde4_add_executable(calligragemini ${calligragemini_SRCS})

target_link_libraries(calligragemini
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTOPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${GLEW_LIBRARIES}
    komain
    wordsprivate
    calligrastageprivate
)

install(TARGETS   calligragemini calligrageminithumbnailhelper ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  calligragemini.desktop               DESTINATION ${XDG_APPS_INSTALL_DIR})
#install(FILES     calligrageminirc calligrageminipanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
install(FILES     calligragemini.qml                   DESTINATION ${DATA_INSTALL_DIR}/calligragemini)
#install(FILES     calligragemini.rc                    DESTINATION ${DATA_INSTALL_DIR}/calligragemini)
install(DIRECTORY qml                                       DESTINATION ${DATA_INSTALL_DIR}/calligragemini)
install(DIRECTORY themes                                    DESTINATION ${DATA_INSTALL_DIR}/calligragemini)

add_subdirectory(pics)
