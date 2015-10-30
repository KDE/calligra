set(dropbox_lib_SRCS
    3rdparty/droprestapi.cc
    3rdparty/filetransferitem.cc
    3rdparty/folderitem.cc
    3rdparty/json.cc
    3rdparty/listmodel.cc
    3rdparty/networkcontroller.cc
    3rdparty/oauth.cc
    3rdparty/options.cc
)

set(dropbox_plugin_SRCS
    controller.cc
    dropboxplugin.cpp)

add_library(calligrageminidropboxplugin SHARED ${dropbox_plugin_SRCS} ${dropbox_lib_SRCS})

target_link_libraries(calligrageminidropboxplugin
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
    ${QT_QTNETWORK_LIBRARIES}
)

install(TARGETS calligrageminidropboxplugin DESTINATION ${QML_INSTALL_DIR}/Calligra/Gemini/Dropbox)
install(FILES   qmldir                      DESTINATION ${QML_INSTALL_DIR}/Calligra/Gemini/Dropbox)
