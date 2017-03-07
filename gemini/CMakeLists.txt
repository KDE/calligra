project(calligragemini)

# TEMPORARY: for  Qt5/KF5 build porting phase deprecation warnings are only annoying noise
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
endif ()

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

if (WIN32)
    set(calligragemini_SRCS ${calligragemini_SRCS} calligrageminiwin.rc)
else ()
    file(GLOB ICONS_SRCS "pics/*-apps-calligragemini.png")
    ecm_add_app_icon(calligragemini_SRCS ICONS ${ICONS_SRCS})
endif ()

# Thumbnail helper application
# =============================================================================

add_executable(calligrageminithumbnailhelper ${thumbnailhelper_SRCS})
target_link_libraries(calligrageminithumbnailhelper
    komain
    KF5::Crash # needed to explicitely disable drkonqi if any plugin links to kdelibs4support
)

# Calligra Gemini application
# =============================================================================

add_executable(calligragemini ${calligragemini_SRCS})

target_link_libraries(calligragemini
    Qt5::Quick
    Qt5::Qml
    Qt5::Gui
    Qt5::Core
    KF5::IconThemes
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
if(APPLE)
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligragemini_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligragemini.app/Contents/Resources)
endif()

add_subdirectory(pics)
add_subdirectory(cloud)
