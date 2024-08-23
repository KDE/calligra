project(calligragemini)

# TEMPORARY: for  Qt6/KF6 build porting phase deprecation warnings are only annoying noise
# if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    # add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
# endif ()
add_subdirectory(lib)

if (WIN32)
    option(ENABLE_GEMINI_STARTUP_MAGIC "Enable Gemini's windows startup magic" ON)

    if (ENABLE_GEMINI_STARTUP_MAGIC)
        add_definitions(-DGEMINI_ENABLE_STARTUP_MAGIC)
    endif ()
endif ()

include_directories(${KOMAIN_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/libs/pageapp
                    ${CMAKE_SOURCE_DIR}/stage
                    ${CMAKE_SOURCE_DIR}/stage/part
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
#     TouchDeclarativeView.cpp
    PropertyContainer.cpp
    Theme.cpp
    ToolManager.cpp
    ParagraphStylesModel.cpp
    KeyboardModel.cpp
    ScribbleArea.cpp
    RecentImageImageProvider.cpp
    RecentFilesModel.cpp
    TemplatesModel.cpp
    TemplateVariantsModel.cpp
    CloudAccountsModel.cpp
)

set(thumbnailhelper_SRCS
    thumbnailhelper.cpp
    ThumbnailHelperImpl.cpp
)

file(GLOB ICONS_SRCS "pics/*-apps-calligragemini.png")
ecm_add_app_icon(calligragemini_SRCS ICONS ${ICONS_SRCS})

# Thumbnail helper application
# =============================================================================

add_executable(calligrageminithumbnailhelper ${thumbnailhelper_SRCS})
target_link_libraries(calligrageminithumbnailhelper
    komain
    KF6::Crash # needed to explicitly disable drkonqi if any plugin links to kdelibs4support
)

# Calligra Gemini application
# =============================================================================

add_executable(calligragemini ${calligragemini_SRCS})

target_link_libraries(calligragemini
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Qml
    Qt6::Gui
    Qt6::Core
    KF6::IconThemes
    komain
    wordsprivate
    calligrastageprivate
    gemini
)

install(TARGETS   calligragemini calligrageminithumbnailhelper ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  org.kde.calligra.gemini.desktop               DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.calligra.gemini.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
#install(FILES     calligrageminirc calligrageminipanelsrc DESTINATION ${KDE_INSTALL_CONFDIR} )
install(FILES     calligragemini.qml                   DESTINATION ${KDE_INSTALL_DATADIR}/calligragemini)
#install(FILES     calligragemini.rc                    DESTINATION ${KDE_INSTALL_DATADIR}/calligragemini)
install(DIRECTORY qml                                       DESTINATION ${KDE_INSTALL_DATADIR}/calligragemini)
install(DIRECTORY themes                                    DESTINATION ${KDE_INSTALL_DATADIR}/calligragemini)
if(APPLE)
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligragemini_SRCS.icns DESTINATION ${KDE_INSTALL_BUNDLEDIR}/calligragemini.app/Contents/Resources)
endif()

add_subdirectory(pics)
add_subdirectory(cloud)
