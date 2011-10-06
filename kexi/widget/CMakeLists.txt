add_subdirectory( utils )
add_subdirectory( dataviewcommon )

IF(KEXI_MOBILE)

ELSE(KEXI_MOBILE)
  add_subdirectory( tableview )
  add_subdirectory( relations )
ENDIF(KEXI_MOBILE)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44023)

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  ${CMAKE_SOURCE_DIR}/kexi/kexidb )


########### next target ###############

set(kexiextendedwidgets_LIB_SRCS
    fields/KexiFieldComboBox.cpp
    fields/KexiFieldListModel.cpp
    fields/KexiFieldListModelItem.cpp
    fields/KexiFieldListView.cpp

    navigator/KexiProjectModel.cpp
    navigator/KexiProjectModelItem.cpp
    navigator/KexiProjectItemDelegate.cpp
    navigator/KexiProjectNavigator.cpp
    navigator/KexiProjectTreeView.cpp

    properties/KexiCustomPropertyFactory.cpp
    properties/KexiCustomPropertyFactory_p.cpp
    properties/KexiPropertyEditorView.cpp
    properties/KexiPropertyPaneViewBase.cpp

    kexiquerydesignersqleditor.cpp 
    kexiqueryparameters.cpp
    kexisectionheader.cpp
    kexidbdrivercombobox.cpp 
    kexieditor.cpp 
    KexiDataSourceComboBox.cpp 
    KexiObjectInfoLabel.cpp
    kexicharencodingcombobox.cpp
    KexiDBTitlePage.cpp
    KexiProjectSelectorWidget.cpp
  )

IF (KEXI_MOBILE)

ELSE (KEXI_MOBILE)
  LIST(APPEND kexiextendedwidgets_LIB_SRCS
    navigator/KexiProjectListView.cpp
    navigator/KexiProjectListViewItem.cpp
    
    kexiscrollview.cpp 
    kexidbconnectionwidget.cpp 
  #   pixmapcollection.cpp 
# TODO replace use of KexiProjectListView and KexiProjectListViewList (with KexiProjectNavigator) 
#      in kexiactionselectiondialog and remove them
    kexiprjtypeselector.cpp
    KexiConnectionSelectorWidget.cpp
    KexiFileWidget.cpp
  )
  kde4_add_ui_files(kexiextendedwidgets_LIB_SRCS
    KexiConnectionSelector.ui
    kexidbconnectionwidget.ui
    kexidbconnectionwidgetdetails.ui
    kexiprjtypeselector.ui
  )
ENDIF (KEXI_MOBILE)

kde4_add_ui_files(kexiextendedwidgets_LIB_SRCS
    KexiDBTitlePage.ui
    KexiProjectSelector.ui
)

kde4_add_library(kexiextendedwidgets SHARED ${kexiextendedwidgets_LIB_SRCS})

target_link_libraries(
    kexiextendedwidgets
    kexidataviewcommon
    kexiutils
    kexiguiutils
    kexicore
    ktexteditor
    kexidb
    ${KOPROPERTY_LIBS}
    ${KDE4_KDEUI_LIBS}
)

IF (KEXI_MOBILE)

ELSE(KEXI_MOBILE)
  target_link_libraries(
    kexiextendedwidgets
    ${KDE4_KFILE_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )
ENDIF(KEXI_MOBILE)

set_target_properties(kexiextendedwidgets PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexiextendedwidgets  ${INSTALL_TARGETS_DEFAULT_ARGS})
