
set(kwidgetsaddons_SRCS
  kacceleratormanager.cpp
  kcolorbutton.cpp
  kfontaction.cpp
  fonthelpers.cpp
  kfontsizeaction.cpp
  kselectaction.cpp
  kdragwidgetdecorator.cpp
  kdualaction.cpp
  kfontchooser.cpp
  kguiitem.cpp
  kled.cpp
  kmessagebox.cpp
  kstandardguiitem.cpp
  kurllabel.cpp # Not good enough quality. Needs to use QUrl instead of QString and should not inherit QLabel, but hold it as a member instead.
  kcapacitybar.cpp
  kfontrequester.cpp
  kpassworddialog.cpp
  kruler.cpp
  kselector.cpp
  kxyselector.cpp
  kseparator.cpp
  ksqueezedtextlabel.cpp
  ktitlewidget.cpp
  ktoggleaction.cpp
  ktogglefullscreenaction.cpp
  kviewstateserializer.cpp
  kviewstatemaintainerbase.cpp
  keditlistwidget.cpp
  kcursor.cpp
  kratingwidget.cpp
  kratingpainter.cpp
  kactionselector.cpp
  kcolorcombo.cpp
  ktoolbarspaceraction.cpp
  kpixmapregionselectordialog.cpp
  kpixmapregionselectorwidget.cpp
  kpassivepopup.cpp
  kactionmenu.cpp
  ktoolbarlabelaction.cpp
  ktoolbarpopupaction.cpp
  kpagedialog.cpp
  kpagemodel.cpp
  kpageview.cpp
  kpageview_p.cpp
  kpagewidget.cpp
  kpagewidgetmodel.cpp
  kassistantdialog.cpp
  kmessagewidget.cpp
  knewpassworddialog.cpp
)

set(kwidgetsaddon_UI
   knewpassworddialog.ui
   kpassworddialog.ui
)

qt5_wrap_ui(kwidgetsaddons_SRCS ${kwidgetsaddon_UI} )

add_library(KWidgetsAddons ${kwidgetsaddons_SRCS})
generate_export_header(KWidgetsAddons)

target_link_libraries(KWidgetsAddons Qt5::Widgets)

if(IS_ABSOLUTE "${INCLUDE_INSTALL_DIR}")
  target_include_directories(KWidgetsAddons INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>" )
else()
  target_include_directories(KWidgetsAddons INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}>" )
endif()

set_target_properties(KWidgetsAddons PROPERTIES VERSION   ${KWIDGETSADDONS_VERSION_STRING}
                                                SOVERSION ${KWIDGETSADDONS_SOVERSION}
)

install(TARGETS KWidgetsAddons EXPORT KWidgetsAddonsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  kacceleratormanager.h
  kcolorbutton.h
  kdragwidgetdecorator.h
  kdualaction.h
  kfontaction.h
  kfontchooser.h
  kfontsizeaction.h
  kguiitem.h
  kled.h
  kmessagebox.h
  kmessageboxdontaskagaininterface.h
  kselectaction.h
  kstandardguiitem.h
  kurllabel.h
  kcapacitybar.h
  kfontrequester.h
  kpassworddialog.h
  kruler.h
  kselector.h
  ktitlewidget.h
  kxyselector.h
  kseparator.h
  ksqueezedtextlabel.h
  ktoggleaction.h
  ktogglefullscreenaction.h
  kviewstateserializer.h
  kviewstatemaintainerbase.h
  keditlistwidget.h
  kcursor.h
  kratingpainter.h
  kratingwidget.h
  kactionselector.h
  kcolorcombo.h
  kpassivepopup.h
  kactionmenu.h
  ktoolbarlabelaction.h
  ktoolbarpopupaction.h
  ktoolbarspaceraction.h
  kpagedialog.h
  kpagedialog_p.h
  kpagemodel.h
  kpageview.h
  kpagewidget.h
  kpagewidgetmodel.h
  kassistantdialog.h
  kmessagewidget.h
  knewpassworddialog.h
  ${CMAKE_CURRENT_BINARY_DIR}/kwidgetsaddons_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
