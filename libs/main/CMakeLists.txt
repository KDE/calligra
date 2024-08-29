if (BUILD_TESTING)
    add_subdirectory( tests )
endif ()

kde_enable_exceptions()

if (APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
endif ()

include_directories( ${KOMAIN_INCLUDES})

########### next target ###############

set(komain_LIB_SRCS
    KoAutoSaveRecoveryDialog.cpp
    KoApplication.cpp
    KoComponentData.cpp
    KoDockerManager.cpp
    KoDocument.cpp
    KoDocumentEntry.cpp
    KoMainWindow.cpp
    KoPrintingDialog.cpp
    KoPrintJob.cpp
    KoVersionDialog.cpp
    KoView.cpp
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
    config/KoConfigDocumentPage.cpp
    config/KoConfigGridPage.cpp
    config/KoConfigMiscPage.cpp

    KoDocumentSectionDelegate.cpp
    KoDocumentSectionToolTip.cpp
    KoDocumentSectionView.cpp

    KoDetailsPane.cpp
    KoOpenPane.cpp
    KoRecentDocumentsPane.cpp
    KoShapeCollectionMenu.cpp
    KoTemplate.cpp
    KoTemplateCreateDia.cpp
    KoTemplateGroup.cpp
    KoTemplates.cpp
    KoTemplatesPane.cpp
    KoTemplateTree.cpp

    KoFindBase.cpp
    KoFindMatch.cpp
    KoFindText.cpp
    KoFindOption.cpp
    KoFindOptionSet.cpp
    KoFindStyle.cpp

    KoPart.cpp

    MainDebug.cpp
)

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
    set(komain_LIB_SRCS ${komain_LIB_SRCS}
        KoApplicationAdaptor.cpp
        KoViewAdaptor.cpp
        KoPartAdaptor.cpp
    )
endif()

ki18n_wrap_ui( komain_LIB_SRCS
   KoOpenPaneBase.ui
   KoDetailsPaneBase.ui
)

qt_wrap_cpp(komain_HEADERS_MOC KoDocumentSectionPropertyAction_p.h)

add_library(komain SHARED ${komain_HEADERS_MOC}  ${komain_LIB_SRCS})
generate_export_header(komain
    EXPORT_FILE_NAME komain_generated_export.h
    )

target_link_libraries(komain
    PUBLIC
        kowidgets
        kotextlayout
    PRIVATE
        koplugin
        KF6::Notifications
        KF6::CoreAddons
        KF6::ItemViews
        KF6::KIOFileWidgets
        KF6::KIOCore
        KF6::Completion
        KF6::IconThemes
        KF6::IconWidgets
        KF6::Crash
)
if (TARGET KF6::DBusAddons)
    target_link_libraries(komain
        PRIVATE
            KF6::DBusAddons
    )
endif()

if (APPLE)
    target_link_libraries(komain PRIVATE ${FOUNDATION_LIBRARY})
endif ()

set_target_properties(komain PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS komain ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# calligradocker.desktop

install(FILES calligra_shell.rc DESTINATION ${KDE_INSTALL_DATADIR}/calligra )

if (APPLE)
    install(FILES osx.stylesheet DESTINATION ${KDE_INSTALL_DATADIR}/calligra )
endif ()

if (SHOULD_BUILD_DEVEL_HEADERS)

# For now, don't install the header files because of the conflict with the komvc
# library.
if (FALSE)
install( FILES
    KoApplication.h
    KoComponentData.h
    KoDocument.h
    KoDocumentEntry.h
    KoDocumentSectionModel.h
    KoFilter.h
    KoFilterChain.h
    KoFilterChainLinkList.h
    KoFilterEntry.h
    KoFilterGraph.h
    KoFilterManager.h
    KoFilterVertex.h
    KoFindBase.h
    KoFindText.h
    KoFindMatch.h
    KoFindOption.h
    KoFindOptionSet.h
    KoMainWindow.h
    KoPageWidgetItem.h
    MacSupport.h
    KoPart.h
    KoPrintJob.h
    KoVersionDialog.h
    KoView.h
    KoStandardAction.h
    komain_export.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
    install( FILES
        KoApplicationAdaptor.h
        KoViewAdaptor.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)
endif()
endif()

endif()
