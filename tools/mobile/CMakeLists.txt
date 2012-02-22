cmake_minimum_required(VERSION 2.6)

add_definitions(-DNO_PIGMENT)

set(KDE_MIN_VERSION "4.3.0")
find_package(KDE4 4.3.0 REQUIRED)
include(KDE4Defaults)

find_package(Qt4 4.5.0 REQUIRED)
SET(QT_USE_QTDBUS 1)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTMAEMO5 TRUE)
SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

find_package(LibXml2 REQUIRED) # f-office uses xml2 directly

include_directories(
    ${CMAKE_BINARY_DIR}
    ${KOABSTRACTION_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${KOPAGEAPP_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/tables/part
    ${CMAKE_SOURCE_DIR}/tables
    ${LIBXML2_INCLUDE_DIR}
    ${KDE4_INCLUDES}
)

SET(CalligraMobile_RCCS CalligraMobile.qrc)
QT4_ADD_RESOURCES(CalligraMobile_RCC_SRCS ${CalligraMobile_RCCS})

find_library( QT_QMAEMO5_LIBRARY QtMaemo5 HINTS ${QT_LIBRARIES} )

set(CalligraMobileMocSrc_SRCS
    ApplicationController.h
    MainWindow.h
    Accelerator.h 
    VirtualKeyBoard.h 
    VirtualKeyBoardLanguageDownloader.h
    ZoomDialog.h 
    HildonMenu.h 
    HildonApplication.h 
    Splash.h 
    NotifyDialog.h 
    DBusAdaptor.h 
    PresentationTool.h 
    AboutDialog.h 
    CollabDialog.h 
    Collaborate.h 
    CollabServer.h 
    CollabClient.h  
    PreviewDialog.h 
    MainWindowAdaptor.h 
    PresentationToolAdaptor.h 
    NotesDialog.h 
    PreviewButton.h 
    SlidingMotionDialog.h 
    FoExternalEditor.h 
    #TODO re-add FoCellEditor.h 
    DigitalSignatureDialog.h
    ConfirmationDialog.h
    FileChooserDialog.h
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoAbstractApplicationController.h
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/RemoveSheetCommand.h
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoCellToolFactory.h
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoCellTool.h
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/FoCellEditor.h
)

if(QT_QTOPENGL_FOUND)
  list(APPEND CalligraMobileMocSrc_SRCS GlWidget.h GlPresenter.h)
endif(QT_QTOPENGL_FOUND)

kde4_add_ui_files(CalligraMobileUiSrc MainWindow.ui ZoomDialog.ui FileChooserDialog.ui AboutDialog.ui CollabDialog.ui)

QT4_WRAP_CPP(CalligraMobileMocSrc ${CalligraMobileMocSrc_SRCS})

set(CalligraMobile_SRCS
    Main.cpp
    ApplicationController.cpp
    ConfirmationDialog.cpp
    MainWindow.cpp 
    Accelerator.cpp 
    VirtualKeyBoard.cpp 
    VirtualKeyBoardLanguageDownloader.cpp
    ZoomDialog.cpp 
    HildonMenu.cpp 
    HildonApplication.cpp 
    Splash.cpp 
    NotifyDialog.cpp 
    PresentationTool.cpp 
    DBusAdaptor.cpp 
    AboutDialog.cpp 
    CollabDialog.cpp
    Collaborate.cpp 
    CollabServer.cpp 
    CollabClient.cpp 
    FlowLayout.cpp 
    PreviewDialog.cpp 
    MainWindowAdaptor.cpp 
    PresentationToolAdaptor.cpp 
    NotesDialog.cpp
    PreviewButton.cpp 
    SlidingMotionDialog.cpp 
    FoExternalEditor.cpp 
    #TODO re-add FoCellEditor.cpp
    DigitalSignatureDialog.cpp
    FoImageSelectionWidget.cpp
    FileChooserDialog.cpp
    ${CalligraMobileUiSrc}
    ${CalligraMobileMocSrc}
    ${CalligraMobile_RCC_SRCS}
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoAbstractApplicationController.cpp
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/RemoveSheetCommand.cpp
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoCellToolFactory.cpp
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/KoCellTool.cpp
    #${CMAKE_SOURCE_DIR}/tools/koabstraction/FoCellEditor.cpp
)

if(QT_QTOPENGL_FOUND)
    list(APPEND CalligraMobile_SRCS
        GlWidget.cpp
        GlPresenter.cpp
    )
endif(QT_QTOPENGL_FOUND)

add_executable(calligramobile ${CalligraMobile_SRCS})

target_link_libraries(calligramobile
    ${LIBXML2_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${X11_X11_LIB}
    ${QT_LIBRARIES}
    ${QT_QTSQL_LIBRARY}
    ${QT_QTNETWORK_LIBRARIES}
    koabstraction
    komain
    kopageapp
    wordsprivate
    calligratablescommon
)

if (QT_QTOPENGL_FOUND)
    target_link_libraries(calligramobile
        ${QT_QTOPENGL_LIBRARY}
        ${OPENGL_LIBRARIES}
    )
endif (QT_QTOPENGL_FOUND)

if(QT_QMAEMO5_LIBRARY)
    target_link_libraries(calligramobile
        ${QT_QMAEMO5_LIBRARY}
        ${SOPRANO_LIBRARIES}
    )
endif(QT_QMAEMO5_LIBRARY)

install(TARGETS calligramobile RUNTIME DESTINATION bin)
install(FILES calligramobile.desktop DESTINATION share/applications/hildon)
install(FILES images/178x200/calligramobile.png DESTINATION share/icons/hicolor/178x200/apps)
install(FILES images/64x64/calligramobile.png DESTINATION share/icons/hicolor/64x64/apps)
install(FILES com.nokia.CalligraMobile.service DESTINATION share/dbus-1/services)
install(FILES images/48x48/Document.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES images/48x48/Presenter.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES images/48x48/SpreadSheet.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES templates/NewDocument.odt DESTINATION share/calligramobile-templates/.templates)
install(FILES templates/Text_Only.odp DESTINATION share/calligramobile-templates/.presentationtemplates)
install(FILES templates/Text_With_Chart.odp DESTINATION share/calligramobile-templates/.presentationtemplates)
install(FILES templates/Text_With_Header.odp DESTINATION share/calligramobile-templates/.presentationtemplates)
install(FILES templates/Text_With_Image.odp DESTINATION share/calligramobile-templates/.presentationtemplates)
install(FILES templates/Text_With_Outline.odp DESTINATION share/calligramobile-templates/.presentationtemplates)
install(FILES templates/NewSpread.ods DESTINATION share/calligramobile-templates/.templates)
if(QT_QMAEMO5_LIBRARY)
    install(FILES Acceleration/mce.ini DESTINATION /etc/mce)
endif(QT_QMAEMO5_LIBRARY)
install(FILES OfficeInterface.h DESTINATION include/office)
