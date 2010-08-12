cmake_minimum_required(VERSION 2.6)

add_definitions(-DNO_PIGMENT)

find_package(Qt4 4.5.0 REQUIRED)
SET(QT_USE_QTDBUS 1)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTMAEMO5 TRUE)
INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_BINARY_DIR} ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/part ${CMAKE_SOURCE_DIR}/kspread/part ${CMAKE_SOURCE_DIR}/kspread)

SET(FreOffice_RCCS FreOffice.qrc)
QT4_ADD_RESOURCES(FreOffice_RCC_SRCS ${FreOffice_RCCS})

QT4_WRAP_UI(FreOfficeUiSrc MainWindow.ui ZoomDialog.ui AboutDialog.ui CollabDialog.ui)

QT4_WRAP_CPP(FreOfficeMocSrc MainWindow.h VirtualKeyBoard.h ZoomDialog.h HildonMenu.h HildonApplication.h Splash.h NotifyDialog.h DBusAdaptor.h PresentationTool.h AboutDialog.h OfficeInterface.h CollabDialog.h Collaborate.h CollabServer.h CollabClient.h  FlowLayout.h PreviewDialog.h MainWindowAdaptor.h PresentationToolAdaptor.h NotesDialog.h PreviewButton.h FoCellToolFactory.h FoCellTool.h SlidingMotionDialog.h GlWidget.h GlPresenter.h FoExternalEditor.h FoCellEditor.h)

add_executable(FreOffice Main.cpp MainWindow.cpp VirtualKeyBoard.cpp ZoomDialog.cpp HildonMenu.cpp HildonApplication.cpp Splash.cpp NotifyDialog.cpp PresentationTool.cpp DBusAdaptor.cpp AboutDialog.cpp CollabDialog.cpp Collaborate.cpp CollabServer.cpp CollabClient.cpp  FlowLayout.cpp PreviewDialog.cpp MainWindowAdaptor.cpp PresentationToolAdaptor.cpp NotesDialog.cpp PreviewButton.cpp FoCellTool.cpp FoCellToolFactory.cpp SlidingMotionDialog.cpp GlWidget.cpp GlPresenter.cpp FoExternalEditor.cpp FoCellEditor.cpp

  ${FreOfficeUiSrc} ${FreOfficeMocSrc} ${FreOffice_RCC_SRCS})

find_library( QT_QMAEMO5_LIBRARY QtMaemo5 HINTS ${QT_LIBRARIES} )

if(QT_QMAEMO5_LIBRARY)
    target_link_libraries(FreOffice ${QT_LIBRARIES} ${QT_QMAEMO5_LIBRARY} ${QT_QTNETWORK_LIBRARIES} ${QT_QTOPENGL_LIBRARY} ${X11_X11_LIB} komain kopageapp kwordprivate kspreadcommon)
else(QT_QMAEMO5_LIBRARY)
    target_link_libraries(FreOffice ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${QT_QTOPENGL_LIBRARY} ${X11_X11_LIB} komain kopageapp kwordprivate kspreadcommon)
endif(QT_QMAEMO5_LIBRARY)

install(TARGETS FreOffice RUNTIME DESTINATION bin)
install(FILES FreOffice.desktop DESTINATION share/applications/hildon)
install(FILES images/178x200/freoffice.png DESTINATION share/icons/hicolor/178x200/apps)
install(FILES images/64x64/freoffice.png DESTINATION share/icons/hicolor/64x64/apps)
install(FILES com.nokia.FreOffice.service DESTINATION share/dbus-1/services)
install(FILES images/48x48/Document.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES images/48x48/Presenter.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES images/48x48/SpreadSheet.png DESTINATION share/icons/hicolor/48x48/hildon)
install(FILES templates/NewDocument.odt DESTINATION share/freoffice-templates/.templates)
install(FILES templates/Text_Only.odp DESTINATION share/freoffice-templates/.presentationtemplates)
install(FILES templates/Text_With_Chart.odp DESTINATION share/freoffice-templates/.presentationtemplates)
install(FILES templates/Text_With_Header.odp DESTINATION share/freoffice-templates/.presentationtemplates)
install(FILES templates/Text_With_Image.odp DESTINATION share/freoffice-templates/.presentationtemplates)
install(FILES templates/Text_With_Outline.odp DESTINATION share/freoffice-templates/.presentationtemplates)
install(FILES templates/NewSpread.ods DESTINATION share/freoffice-templates/.templates)

install(FILES OfficeInterface.h DESTINATION include/office)
SET(CMAKE_CXX_FLAGS "-g -Wall")
