include_directories(
        ${KOODF_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon/
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_SOURCE_DIR}/karbon/ui/dialogs
        ${CMAKE_SOURCE_DIR}/karbon/ui/dockers
        ${CMAKE_SOURCE_DIR}/karbon/ui/widgets
        ${CMAKE_SOURCE_DIR}/karbon/ui/commands
)

set(karbonui_SRCS
    KarbonFactory.cpp
    KarbonDocument.cpp
    KarbonPart.cpp
    KarbonView.cpp 
    KarbonPrintJob.cpp
    KarbonZoomController.cpp
    KarbonDocumentMergeCommand.cpp
    
    commands/KarbonLayerReorderCommand.cpp
    
    dialogs/KarbonConfigureDialog.cpp

    dockers/KarbonLayerDocker.cpp
    dockers/KarbonLayerModel.cpp
    dockers/KarbonLayerSortingModel.cpp

    widgets/KarbonCanvas.cpp
    widgets/KarbonGradientWidget.cpp
    widgets/KarbonSmallStylePreview.cpp
    widgets/KarbonPaletteBarWidget.cpp
    widgets/KarbonPaletteWidget.cpp
    widgets/KarbonConfigInterfacePage.cpp
)

add_library( karbonui SHARED ${karbonui_SRCS} )
generate_export_header(karbonui BASE_NAME karbonui)

target_link_libraries(karbonui
    PUBLIC
        karboncommon
        flake
    PRIVATE
        kowidgets
        koodf
        KF5::KDELibs4Support
)

set_target_properties(karbonui PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS karbonui  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

