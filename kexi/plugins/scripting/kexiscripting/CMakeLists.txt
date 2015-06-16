include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/widget

    ${KOKROSS_INCLUDES}
)

# the main plugin
set(kexi_scriptplugin_SRCS
    kexiscriptpart.cpp
    kexiscripteditor.cpp
    kexiscriptdesignview.cpp
)

qt5_wrap_cpp(kexi_scriptplugin_SRCS kexiscriptadaptor.h)

add_library(kexi_scriptplugin MODULE ${kexi_scriptplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_scriptplugin kexi_scriptplugin.desktop)

target_link_libraries(kexi_scriptplugin
    PRIVATE
        kexiextendedwidgets
        kokross
        kexicore
        kexiutils

        KDb
        KProperty

        KF5::KIOWidgets
        Qt5::Gui
)
install(TARGETS kexi_scriptplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
