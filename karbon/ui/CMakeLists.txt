include_directories( 
        ${KDE4_INCLUDE_DIR} 
        ${QT_INCLUDES} 
        ${KOMAIN_INCLUDES}         
        ${CMAKE_SOURCE_DIR}/karbon/
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_SOURCE_DIR}/karbon/ui/dialogs
        ${CMAKE_SOURCE_DIR}/karbon/ui/dockers
        ${CMAKE_SOURCE_DIR}/karbon/ui/widgets
)

set(karbonui_SRCS
    KarbonFactory.cpp
    KarbonPart.cpp 
    KarbonView.cpp 
    KarbonPrintJob.cpp
    KarbonZoomController.cpp
    KarbonDocumentMergeCommand.cpp
    
    dialogs/KarbonConfigureDialog.cpp

    dockers/KarbonLayerDocker.cpp
    dockers/KarbonLayerModel.cpp
    dockers/KarbonLayerSortingModel.cpp

    widgets/KarbonCanvas.cpp
    widgets/KarbonGradientWidget.cpp
    widgets/KarbonGradientEditWidget.cpp
    widgets/KarbonStylePreview.cpp
    widgets/KarbonSmallStylePreview.cpp
    widgets/KarbonStyleButtonBox.cpp
    widgets/KarbonPatternOptionsWidget.cpp
)

kde4_add_ui_files(karbonui_SRCS
    widgets/KarbonPatternOptionsWidget.ui
)


kde4_add_library( karbonui SHARED ${karbonui_SRCS} )

target_link_libraries( karbonui ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} ${QT_QT3SUPPORT_LIBRARY}
                                karboncommon kostore pigmentcms
                                komain koresources kocolorwidgets
                                
                                kotext
                                flake )

set_target_properties(karbonui PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS karbonui  ${INSTALL_TARGETS_DEFAULT_ARGS})

