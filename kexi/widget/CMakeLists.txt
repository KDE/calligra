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
    kexiquerydesignersqleditor.cpp 
    kexiqueryparameters.cpp
    kexisectionheader.cpp
    kexidbdrivercombobox.cpp 
    kexieditor.cpp 
    KexiFieldListModel.cpp
    KexiFieldListModelItem.cpp
    kexifieldlistview.cpp 
    kexifieldcombobox.cpp 
    kexidatasourcecombobox.cpp 
    KexiObjectInfoLabel.cpp
    KexiPropertyPaneViewBase.cpp
    KexiPropertyEditorView.cpp
    kexicustompropertyfactory.cpp 
    kexicustompropertyfactory_p.cpp 
    kexicharencodingcombobox.cpp
    KexiProjectModel.cpp
    KexiProjectModelItem.cpp
    KexiProjectTreeView.cpp
    KexiProjectNavigator.cpp
  )

IF (KEXI_MOBILE)

ELSE (KEXI_MOBILE)
  LIST(APPEND kexiextendedwidgets_LIB_SRCS
    kexidataawareview.cpp 
    kexidatatable.cpp 
    kexiscrollview.cpp 
    kexidbconnectionwidget.cpp 
  #   pixmapcollection.cpp 
    KexiProjectListView.cpp 
    KexiProjectListViewItem.cpp
    kexiprjtypeselector.cpp
  )
ENDIF (KEXI_MOBILE)




kde4_add_ui_files(kexiextendedwidgets_LIB_SRCS
  kexidbconnectionwidget.ui
  kexidbconnectionwidgetdetails.ui
  kexiprjtypeselector.ui
)

IF (KEXI_MOBILE)

ELSE(KEXI_MOBILE)
  #LIST(APPEND kexiextendedwidgets_LIB_SRCS
  #  kexiprjtypeselector.ui
  #)
ENDIF(KEXI_MOBILE)

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
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )
ENDIF(KEXI_MOBILE)

set_target_properties(kexiextendedwidgets PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kexiextendedwidgets  ${INSTALL_TARGETS_DEFAULT_ARGS})
