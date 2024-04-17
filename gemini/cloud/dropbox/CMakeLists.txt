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

if(WIN32)
add_library(calligrageminidropboxplugin MODULE ${dropbox_plugin_SRCS} ${dropbox_lib_SRCS})
else()
add_library(calligrageminidropboxplugin SHARED ${dropbox_plugin_SRCS} ${dropbox_lib_SRCS})
endif()

target_link_libraries(calligrageminidropboxplugin
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
)

install(TARGETS calligrageminidropboxplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Dropbox)
install(FILES   qmldir                      DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Dropbox)
