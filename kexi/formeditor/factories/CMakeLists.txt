include_directories(${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core)

add_definitions(-DKEXI_FORMS_NO_LIST_WIDGET)

set(kexiforms_standardwidgetsplugin_SRCS
    KexiStandardFormWidgetsFactory.cpp
    KexiStandardFormWidgets.cpp
    KexiStandardContainerFormWidgets.cpp
)

add_library(kexiforms_standardwidgetsplugin MODULE ${kexiforms_standardwidgetsplugin_SRCS})
kcoreaddons_desktop_to_json(kexiforms_standardwidgetsplugin kexiforms_standardwidgetsplugin.desktop)

target_link_libraries(kexiforms_standardwidgetsplugin
    kformdesigner
    kundo2

    Qt5::Core
    Qt5::Gui
    Qt5::Xml
)

install(TARGETS kexiforms_standardwidgetsplugin DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})
