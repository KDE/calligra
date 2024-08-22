include_directories(
    ${KOPAGEAPP_INCLUDES}
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
add_subdirectory(tools)

### kopageapp ###
set( kopageapp_LIB_SRCS
     PageAppDebug.cpp
     KoPADocument.cpp
     KoShapeTraversal.cpp
     KoPAView.cpp
     KoPAViewBase.cpp
     KoPACanvas.cpp
     KoPACanvasItem.cpp
     KoPACanvasBase.cpp
     KoPASavingContext.cpp
     KoPAPixmapCache.cpp
     KoPAPageBase.cpp
     KoPAMasterPage.cpp
     KoPAPage.cpp
     KoPAPageContainerModel.cpp
     KoPAViewMode.cpp
     KoPAViewModeNormal.cpp
     KoPALoadingContext.cpp
     KoPAOdfPageSaveHelper.cpp
     KoPAPastePage.cpp
     KoPADocumentModel.cpp
     KoPAPageThumbnailModel.cpp
     KoPADocumentStructureDocker.cpp
     KoPAUtil.cpp
     KoPAPrintJob.cpp
     KoPATextPage.cpp
     KoPAPageProvider.cpp
     commands/KoPAPageInsertCommand.cpp
     commands/KoPAPageDeleteCommand.cpp
     commands/KoPAPageMoveCommand.cpp
     commands/KoPAChangeMasterPageCommand.cpp
     commands/KoPAChangePageLayoutCommand.cpp
     commands/KoPADisplayMasterShapesCommand.cpp
     commands/KoPADisplayMasterBackgroundCommand.cpp
     dialogs/KoPAMasterPageDialog.cpp
     dialogs/KoPAPageLayoutDialog.cpp
     dialogs/KoPAConfigureDialog.cpp
     widgets/KoPageNavigator.cpp
     widgets/KoPageNavigatorButton.cpp
     widgets/KoPABackgroundFillWidget.cpp
     tools/backgroundTool/KoPABackgroundTool.cpp
     tools/backgroundTool/KoPABackgroundToolWidget.cpp
)

ki18n_wrap_ui(kopageapp_LIB_SRCS
    tools/backgroundTool/BackgroundToolWidget.ui
)

add_library(kopageapp SHARED ${kopageapp_LIB_SRCS})
generate_export_header(kopageapp
    EXPORT_FILE_NAME kopageapp_generated_export.h)

target_link_libraries(kopageapp
    PUBLIC
        komain
    PRIVATE
        kotextlayout
)
set_target_properties(kopageapp PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS kopageapp  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES
    kopageapp_export.h
    KoPACanvas.h
    KoPACanvasItem.h
    KoPACanvasBase.h
    KoPADocument.h
    KoPageApp.h
    KoPALoadingContext.h
    KoPAPage.h
    KoPAMasterPage.h
    KoPAPageBase.h
    KoPAPageContainerModel.h
    KoPASavingContext.h
    KoPAUtil.h
    KoPAView.h
    KoPAViewBase.h
    KoPAViewMode.h
    KoPAViewModeNormal.h
    tools/backgroundTool/KoPABackgroundTool.h
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)

endif()
