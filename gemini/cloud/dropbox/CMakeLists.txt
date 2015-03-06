set(dropbox_lib_SRCS
    libs/droprestapi.cc
    libs/filetransferitem.cc
    libs/folderitem.cc
    libs/json.cc
    libs/listmodel.cc
    libs/networkcontroller.cc
    libs/oauth.cc
    libs/options.cc
)

set(dropbox_plugin_SRCS
    controller.cc
    dropboxplugin.cpp)

automoc4_add_library(calligrageminidropboxplugin MODULE ${dropbox_plugin_SRCS} ${dropbox_lib_SRCS})

target_link_libraries(calligrageminidropboxplugin
    Qt5::Declarative
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
)

install(TARGETS calligrageminidropboxplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Dropbox)
install(FILES   qmldir                      DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Dropbox)
