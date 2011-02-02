
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/libs/main ${CMAKE_SOURCE_DIR}/kexi/widget/tableview/private ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_form_PART_SRCS kexiforms.cpp )

kde4_add_plugin(kexihandler_form ${kexihandler_form_PART_SRCS})

target_link_libraries(
    kexihandler_form
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kformdesigner
    ${KOPROPERTY_LIBS}
    kexiformutils
    ${KDE4_KDECORE_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kexihandler_form  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_kexidbwidgets_PART_SRCS kexidbfactory.cpp )

kde4_add_plugin(kformdesigner_kexidbwidgets ${kformdesigner_kexidbwidgets_PART_SRCS})



target_link_libraries(
    kformdesigner_kexidbwidgets
    kformdesigner
    kexiformutils
    kexidb
    kexicore
    kexiguiutils
	kexidatatable
	kexiextendedwidgets
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_kexidbwidgets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kexiformutils_LIB_SRCS 
 #  kexiformdataiteminterface.cpp 
   kexidataawarewidgetinfo.cpp  
   kexiformscrollview.cpp 
   kexidbtextwidgetinterface.cpp 
   kexiformmanager.cpp 
   kexidatasourcepage.cpp 
   kexiformpart.cpp 
   kexiformview.cpp

  widgets/kexidbutils.cpp
  widgets/kexidbautofield.cpp
  widgets/kexidbform.cpp
# todo  widgets/kexidbsubform.cpp
  widgets/kexidblabel.cpp
  widgets/kexidbimagebox.cpp
  widgets/kexipushbutton.cpp
  widgets/kexiframe.cpp
  widgets/kexidblineedit.cpp
  widgets/kexidbcheckbox.cpp
  widgets/kexidbtextedit.cpp
  widgets/kexidbcombobox.cpp
)

#obsolete	widgets/kexidbdoublespinbox.cpp
#obsolete	widgets/kexidbintspinbox.cpp
#obsolete	widgets/kexidbdatetimeedit.cpp
#obsolete	widgets/kexidbtimeedit.cpp
#obsolete	widgets/kexidbdateedit.cpp


kde4_add_library(kexiformutils SHARED ${kexiformutils_LIB_SRCS})

target_link_libraries(
    kexiformutils
    kexicore
    kexiextendedwidgets
    kformdesigner
    kexidb
    kexiutils
    kexidatatable
    kexiguiutils
    ${KOPROPERTY_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

set_target_properties(kexiformutils PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kexiformutils  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES  kformdesigner_kexidbfactory.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kformdesigner)
install( FILES  kexiformhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
