include_directories( ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget
    ${CMAKE_SOURCE_DIR}/kexi/formeditor)

if(MARBLE_FOUND)
  add_subdirectory(widgets/mapbrowser)
endif()

if(Qt5WebKitWidgets_FOUND)
  add_subdirectory(widgets/webbrowser)
endif()

# the main plugin
set(kexi_formplugin_SRCS kexiforms.cpp)

add_library(kexi_formplugin MODULE ${kexi_formplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_formplugin kexi_formplugin.desktop)

target_link_libraries(kexi_formplugin
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kformdesigner
    kexiformutils

    KPropertyWidgets
)

install(TARGETS kexi_formplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})

# the form widgets plugin
set(kexiforms_dbwidgetsplugin_SRCS kexidbfactory.cpp)

add_library(kexiforms_dbwidgetsplugin MODULE ${kexiforms_dbwidgetsplugin_SRCS})
kcoreaddons_desktop_to_json(kexiforms_dbwidgetsplugin kexiforms_dbwidgetsplugin.desktop)

target_link_libraries(kexiforms_dbwidgetsplugin
    PRIVATE
        kformdesigner
        kexiformutils
        kexicore
        kexiguiutils
        kexidataviewcommon
        kexidatatable
        kexiextendedwidgets

        KDb

        Qt5::Gui
)

install(TARGETS kexiforms_dbwidgetsplugin DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

# the form utils lib
set(kexiformutils_LIB_SRCS
 #  kexiformdataiteminterface.cpp
   kexidataawarewidgetinfo.cpp
   KexiFormScrollAreaWidget.cpp
   kexiformscrollview.cpp
   kexidbtextwidgetinterface.cpp
   kexiformmanager.cpp
   kexidatasourcepage.cpp
   kexiformpart.cpp
   kexiformview.cpp
   kexidbfactorybase.cpp

  widgets/kexidbutils.cpp
  widgets/kexidbautofield.cpp
  widgets/kexidbform.cpp
  widgets/kexidblabel.cpp
  widgets/kexidbimagebox.cpp
  widgets/KexiDBPushButton.cpp
  widgets/kexiframe.cpp
  widgets/kexidblineedit.cpp
  widgets/kexidbcheckbox.cpp
  widgets/kexidbtextedit.cpp
  widgets/kexidbcombobox.cpp
  widgets/kexidbcommandlinkbutton.cpp
  widgets/kexidbslider.cpp
  widgets/kexidbprogressbar.cpp
  widgets/kexidbdatepicker.cpp
)

    #obsolete    widgets/kexidbdoublespinbox.cpp
    #obsolete    widgets/kexidbintspinbox.cpp

add_library(kexiformutils SHARED ${kexiformutils_LIB_SRCS})

generate_export_header(kexiformutils)

target_link_libraries(kexiformutils
    PRIVATE
        kexicore
        kexiextendedwidgets
        kformdesigner
        kexiutils
        kexidataviewcommon
        kexidatatable
        kexiguiutils

        KDb
        KPropertyWidgets

        Qt5::Gui
        Qt5::Xml
)

set_target_properties(kexiformutils PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
                      SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION})
install(TARGETS kexiformutils  ${INSTALL_TARGETS_DEFAULT_ARGS})
