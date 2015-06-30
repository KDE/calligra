#TODO add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44019)

include_directories(  ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core
${CMAKE_SOURCE_DIR}/kexi/kexiutils
${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/main/startup
${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_BINARY_DIR}/kexi/widget
${CMAKE_SOURCE_DIR}/libs ${KOMAIN_INCLUDES}   )

set(QT_USE_QTUITOOLS true)

set(keximain_LIB_SRCS
    KexiMainWindow.cpp
    KexiMainWindow_p.cpp
    KexiMenuWidget.cpp
    kexifinddialog.cpp
    kexistatusbar.cpp
    #TODO: kexinewstuff.cpp
    KexiSearchLineEdit.cpp
    KexiUserFeedbackAgent.cpp
    KexiBugReportDialog.cpp

    startup/KexiNewProjectAssistant.cpp
    startup/KexiOpenProjectAssistant.cpp
    startup/KexiWelcomeAssistant.cpp
    startup/KexiWelcomeStatusBar.cpp
    startup/KexiImportExportAssistant.cpp
    startup/KexiStartupDialog.cpp
    startup/KexiStartup.cpp
    startup/KexiTemplatesModel.cpp
    startup/KexiRecentProjectsModel.cpp
    startup/KexiAssistantMessageHandler.cpp
    startup/KexiPasswordPage.cpp

    #todo printing/kexisimpleprintingengine.cpp
    #todo printing/kexisimpleprintingpagesetup.cpp
    #todo printing/kexisimpleprintingpart.cpp
    #todo printing/kexisimpleprintpreviewwindow.cpp
)

ki18n_wrap_ui(keximain_LIB_SRCS
    kexifinddialog.ui
    startup/KexiProjectStorageTypeSelectionPage.ui
    startup/KexiServerDBNamePage.ui
    startup/KexiMainImportExportPage.ui
)

add_library(keximain SHARED ${keximain_LIB_SRCS})

generate_export_header(keximain)

target_link_libraries(keximain
PRIVATE
    kexicore
    kexiutils
    kexiextendedwidgets
    kexiguiutils
#TODO: KEXI3    keximigrate

    KDb
    KProperty

    KF5::KIOWidgets
    KF5::XmlGui #KActionCollection...

    Qt5::UiTools
)

set_target_properties(keximain PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS keximain  ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(status)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
