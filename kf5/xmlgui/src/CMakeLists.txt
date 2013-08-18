find_package(X11)
set_package_properties(X11 PROPERTIES TYPE OPTIONAL)

find_package(LibAttica 1.0.0 NO_MODULE)
set_package_properties(LibAttica PROPERTIES DESCRIPTION "A Qt library that implements the Open Collaboration Services API"
                       PURPOSE "Support for Get Hot New Stuff in XMLGUI"
                       URL "https://projects.kde.org/attica"
                       TYPE OPTIONAL
                      )

if (X11_FOUND)
   set (HAVE_X11 1)
   set (XMLGUI_EXTRA_LIBS ${XMLGUI_EXTRA_LIBS} ${X11_LIBRARIES})
else ()
   set (HAVE_X11 0)
endif ()

if (LibAttica_FOUND)
   set (HAVE_ATTICA 1)
   set (XMLGUI_EXTRA_LIBS ${XMLGUI_EXTRA_LIBS} ${LibAttica_LIBRARIES})
   include_directories( ${LibAttica_INCLUDE_DIRS} )
else ()
   set (HAVE_ATTICA 0)
endif ()

configure_file(config-xmlgui.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xmlgui.h )

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/kaboutapplicationconfigattica_p.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/kaboutapplicationconfigattica_p.h
)

set(xmlgui_SRCS
  kaboutapplicationdialog.cpp
  kaboutapplicationpersonlistdelegate_p.cpp
  kaboutapplicationpersonlistview_p.cpp
  kaboutapplicationpersonmodel_p.cpp
  kaboutkdedialog_p.cpp
  kactioncategory.cpp
  kactioncollection.cpp
  kactionconflictdetector.cpp
  kbugreport.cpp
  kedittoolbar.cpp
  kglobalaccel.cpp
  kglobalshortcutinfo.cpp
  kglobalshortcutinfo_dbus.cpp
  khelpmenu.cpp
  kkeysequencewidget.cpp
  kmainwindow.cpp
  kmainwindowiface.cpp
  kmenumenuhandler_p.cpp
  kshortcuteditwidget.cpp
  kshortcutschemeseditor.cpp
  kshortcutschemeshelper.cpp
  kshortcutsdialog.cpp
  kshortcutseditor.cpp
  kshortcutseditordelegate.cpp
  kshortcutseditoritem.cpp
  kshortcutwidget.cpp
  kswitchlanguagedialog_p.cpp
  ktoggletoolbaraction.cpp
  ktoolbar.cpp
  ktoolbarhandler.cpp
  kxmlguibuilder.cpp
  kxmlguiclient.cpp
  kxmlguifactory.cpp
  kxmlguifactory_p.cpp
  kxmlguiversionhandler.cpp
  kxmlguiwindow.cpp
  kundoactions.cpp
  kcheckaccelerators.cpp
  khelpclient.cpp
)

set(kglobalaccel_xml ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.KGlobalAccel.xml)
set_source_files_properties(${kglobalaccel_xml} PROPERTIES INCLUDE "kglobalshortcutinfo_p.h")
qt5_add_dbus_interface(xmlgui_SRCS ${kglobalaccel_xml} kglobalaccel_interface )
install(FILES ${kglobalaccel_xml} DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

set(kglobalaccel_component_xml ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.kglobalaccel.Component.xml)
set_source_files_properties(${kglobalaccel_component_xml} PROPERTIES INCLUDE "kglobalshortcutinfo_p.h")
qt5_add_dbus_interface(xmlgui_SRCS ${kglobalaccel_component_xml} kglobalaccel_component_interface )
install(FILES ${kglobalaccel_component_xml} DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

set( xmlgui_UI
   kshortcutsdialog.ui
   kshortcutwidget.ui
)

qt5_wrap_ui(xmlgui_SRCS ${xmlgui_UI} )

add_library(XmlGui ${xmlgui_SRCS})
generate_export_header(XmlGui)

target_link_libraries(XmlGui LINK_PUBLIC
    Qt5::Widgets
    Qt5::DBus
    Qt5::X11Extras
    Qt5::Xml
    Qt5::Network
    Qt5::PrintSupport

    KWidgetsAddons
    ItemViews
    KI18n
    KCoreAddons
    KConfigCore
    KConfigWidgets
#    KIconThemes
#    KTextWidgets
#    SonnetUi
#    KWindowSystem
    ${XMLGUI_EXTRA_LIBS}
)

set_target_properties(XmlGui PROPERTIES VERSION   ${XMLGUI_VERSION_STRING}
                                        SOVERSION ${XMLGUI_SOVERSION}
)

install(TARGETS XmlGui EXPORT XmlGuiTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  kaboutapplicationdialog.h
  kactioncategory.h
  kactioncollection.h
  kbugreport.h
  kedittoolbar.h
  kglobalaccel.h
  kglobalshortcutinfo.h
  khelpmenu.h
  kkeysequencewidget.h
  kmainwindow.h
  kshortcutsdialog.h
  kshortcutseditor.h
  kshortcutwidget.h
  ktoggletoolbaraction.h
  ktoolbar.h
  kxmlguibuilder.h
  kxmlguiclient.h
  kxmlguifactory.h
  kxmlguiwindow.h
  kundoactions.h
  khelpclient.h
  ${CMAKE_CURRENT_BINARY_DIR}/xmlgui_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)

install( FILES ui_standards.rc  DESTINATION  ${CONFIG_INSTALL_DIR}/ui )

install( FILES
 aboutkde.png
 thumb_frame.png
 DESTINATION  ${DATA_INSTALL_DIR}/xmlgui/pics )
