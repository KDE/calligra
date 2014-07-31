add_subdirectory( tests )

# generate calligraversion.h
configure_file(calligraversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/calligraversion.h)

# generate calligragitversion.h
configure_file(calligragitversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/calligragitversion.h)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

if (APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
endif (APPLE)

include_directories( ${KOMVC_INCLUDES}
)

########### next target ###############

set(komain_LIB_SRCS
    KoAutoSaveRecoveryDialog.cpp
    KoApplication.cpp
    KoDockerManager.cpp
    KoDocument.cpp
    KoDocumentAdaptor.cpp
    KoDocumentEntry.cpp
    KoDocumentInfo.cpp
    KoDocumentInfoDlg.cpp
    KoDocumentInfoPropsPage.cpp
    KoGlobal.cpp
    KoGridData.cpp
    KoMainWindow.cpp
    KoPrintingDialog.cpp
    KoPrintJob.cpp
    KoRulerController.cpp
    KoRuler.cpp
    KoModeBox.cpp
    KoModeBoxDocker.cpp
    KoModeBoxFactory.cpp
    KoToolBox.cpp
    KoToolBoxDocker.cpp
    KoToolBoxFactory.cpp
    KoToolDocker_p.cpp
    KoVersionDialog.cpp
    KoView.cpp
    calligraversion.cpp
    KoFilterManager.cpp
    KoFilterChain.cpp
    KoFilter.cpp
    KoFilterEntry.cpp
    KoFilterManager_p.cpp
    KoFilterVertex.cpp
    KoFilterGraph.cpp
    KoFilterEdge.cpp
    KoFilterChainLink.cpp
    KoFilterChainLinkList.cpp
    KoStandardAction.cpp
    KoUndoStackAction.cpp

    KoDocumentSectionDelegate.cpp
    KoDocumentSectionToolTip.cpp
    KoDocumentSectionView.cpp

    KoDetailsPane.cpp
    KoOpenPane.cpp
    KoRecentDocumentsPane.cpp
    KoTemplate.cpp
    KoTemplateCreateDia.cpp
    KoTemplateGroup.cpp
    KoTemplates.cpp
    KoTemplatesPane.cpp
    KoTemplateTree.cpp

    KoPart.cpp

    thememanager.cpp
    xmlgui/KoMainWindowBase.cpp
    xmlgui/KoMenuMenuHandler_p.cpp
    xmlgui/KoXMLGUIBuilder.cpp
    xmlgui/KoXMLGUIFactory.cpp
    xmlgui/KoXMLGUIVersionHandler.cpp
    xmlgui/KoToolBarHandler.cpp
    xmlgui/KoXMLGUIClient.cpp
    xmlgui/KoXMLGUIFactory_p.cpp
    xmlgui/KoXMLGUIWindow.cpp

)

if( QT_QTDBUS_FOUND )
    set(komain_LIB_SRCS ${komain_LIB_SRCS}
        KoApplicationAdaptor.cpp
        KoViewAdaptor.cpp
        KoPartAdaptor.cpp
    )
endif( QT_QTDBUS_FOUND )

if (APPLE)
    set(komain_LIB_SRCS ${komain_LIB_SRCS}
        MacSupport.mm
    )
endif (APPLE)

kde4_add_ui_files( komain_LIB_SRCS
   koDocumentInfoAboutWidget.ui
   koDocumentInfoAuthorWidget.ui
   KoOpenPaneBase.ui
   KoDetailsPaneBase.ui
)
kde4_add_library(komain SHARED ${komain_LIB_SRCS})

target_link_libraries(komain ${extra_libs}
    ${KDE4_KFILE_LIBRARY}
    kowidgets
    kotextlayout
)

if (APPLE)
    target_link_libraries(komain ${FOUNDATION_LIBRARY})
endif (APPLE)

target_link_libraries(komain LINK_INTERFACE_LIBRARIES kotext kowidgets )

set_target_properties(komain PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS komain ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install(FILES calligradocker.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
install(FILES calligra_shell.rc DESTINATION ${DATA_INSTALL_DIR}/calligra )

if (APPLE)
    install(FILES osx.stylesheet DESTINATION ${DATA_INSTALL_DIR}/calligra )
endif (APPLE)

if( QT_QTDBUS_FOUND )
    install( FILES
        KoApplicationAdaptor.h
        KoViewAdaptor.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)
endif( QT_QTDBUS_FOUND )
