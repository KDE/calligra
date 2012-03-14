add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44019)

include_directories(  ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core 
${CMAKE_SOURCE_DIR}/kexi/kexiutils
${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/main/startup
${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_BINARY_DIR}/kexi/widget
${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/libs ${KOMAIN_INCLUDES} ${KDE4_INCLUDES}  )

ADD_DEFINITIONS(
-DKEXI_NO_CTXT_HELP=1
)

set(QT_USE_QTUITOOLS true)

########### next target ###############

set(keximain_LIB_SRCS 
KexiMainWindow.cpp 
KexiMenuWidget.cpp
kexifinddialog.cpp
kexistatusbar.cpp 
kexinamewidget.cpp 
kexinamedialog.cpp 
kexinewstuff.cpp
KexiSearchLineEdit.cpp
KexiUserFeedbackAgent.cpp

startup/KexiNewProjectAssistant.cpp
startup/KexiOpenProjectAssistant.cpp
startup/KexiWelcomeAssistant.cpp
startup/KexiWelcomeStatusBar.cpp
startup/KexiImportExportAssistant.cpp
startup/KexiStartupFileHandler.cpp
startup/KexiStartupDialog.cpp
startup/KexiStartup.cpp
startup/KexiStartupDialogTemplatesPage.cpp
startup/KexiTemplatesModel.cpp
startup/KexiRecentProjectsModel.cpp

#todo printing/kexisimpleprintingengine.cpp
#todo printing/kexisimpleprintingpagesetup.cpp
#todo printing/kexisimpleprintingpart.cpp
#todo printing/kexisimpleprintpreviewwindow.cpp

)

kde4_add_ui_files(keximain_LIB_SRCS
  kexifinddialog.ui
  startup/KexiProjectStorageTypeSelectionPage.ui
  startup/KexiOpenExistingFile.ui
  startup/KexiServerDBNamePage.ui
  startup/KexiMainImportExportPage.ui
)

kde4_add_library(keximain SHARED ${keximain_LIB_SRCS})

target_link_libraries(
    keximain
    kexidb
    kexicore
    kexiutils
    ${KOPROPERTY_LIBS}
    kexiextendedwidgets
    kexiguiutils
    ${KDE4_KNEWSTUFF_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    ${QT_QTUITOOLS_LIBRARY}
)

set_target_properties(keximain PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS keximain  ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(status)
