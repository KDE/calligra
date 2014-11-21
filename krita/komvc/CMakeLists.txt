# generate calligraversion.h
configure_file(calligraversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/calligraversion.h)

# generate calligragitversion.h
configure_file(calligragitversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/calligragitversion.h)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

if (APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
endif (APPLE)

include_directories( ${KOMVC_INCLUDES}
    ${CURRENT_SOURCE_DIR}/xmlgui
)

########### next target ###############

set(komvc_LIB_SRCS
    KoAutoSaveRecoveryDialog.cpp
    KoApplication.cpp
    KoDockerManager.cpp
    KoDocument.cpp
    KoDocumentEntry.cpp
    KoDocumentInfo.cpp
    KoDocumentInfoDlg.cpp
    KoDocumentInfoPropsPage.cpp
    KoMainWindow.cpp
    KoPrintingDialog.cpp
    KoPrintJob.cpp
    KoToolBox.cpp
    KoToolBoxDocker.cpp
    KoToolBoxFactory.cpp
    KoToolDocker_p.cpp
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
)

if (APPLE)
    set(komvc_LIB_SRCS ${komvc_LIB_SRCS}
        MacSupport.mm
    )
endif (APPLE)

kde4_add_ui_files( komvc_LIB_SRCS
   koDocumentInfoAboutWidget.ui
   koDocumentInfoAuthorWidget.ui
   KoOpenPaneBase.ui
   KoDetailsPaneBase.ui
)
kde4_add_library(komvc SHARED ${komvc_LIB_SRCS})

target_link_libraries(komvc ${extra_libs}
    ${KDE4_KFILE_LIBRARY}
    kowidgets
    kotextlayout
)

if (APPLE)
    target_link_libraries(komvc ${FOUNDATION_LIBRARY})
endif (APPLE)

target_link_libraries(komvc LINK_INTERFACE_LIBRARIES kotext kowidgets )

set_target_properties(komvc PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS komvc ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install(FILES calligradocker.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

if (APPLE)
    install(FILES osx.stylesheet DESTINATION ${DATA_INSTALL_DIR}/calligra )
endif (APPLE)
