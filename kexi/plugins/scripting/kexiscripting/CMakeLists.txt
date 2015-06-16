include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/widget

    ${KOKROSS_INCLUDES}
)

########### next target ###############

set(kexihandler_script_PART_SRCS kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp )
qt5_wrap_cpp(kexihandler_script_PART_SRCS kexiscriptadaptor.h)
add_library(kexihandler_script MODULE ${kexihandler_script_PART_SRCS})
target_link_libraries(
    kexihandler_script
    kexiextendedwidgets
    kokross
    kexicore
    kexiutils

    KDb
    KProperty

    KF5::KIOWidgets
    Qt5::Gui
)

########### install files ###############

install(TARGETS kexi_scriptplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
