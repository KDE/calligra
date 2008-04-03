include_directories( 
        ${KDE4_INCLUDE_DIR} 
        ${QT_INCLUDES} 
        ${KOGUIUTILS_INCLUDES} 
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

    dialogs/KarbonConfigureDialog.cpp

    dockers/KarbonLayerDocker.cpp
    dockers/KarbonLayerModel.cpp
    dockers/KarbonLayerSortingModel.cpp
    dockers/KarbonStylePreviewDocker.cpp

    widgets/KarbonCanvas.cpp
    widgets/vcolorslider.cc
    widgets/vgradientwidget.cc
    widgets/KarbonGradientChooser.cpp
    widgets/KarbonGradientItem.cpp
    widgets/vgradienttabwidget.cc
    widgets/KarbonStylePreview.cpp
    widgets/KarbonStyleButtonBox.cpp
    widgets/KarbonPatternChooser.cpp
    widgets/KarbonPatternItem.cpp
)


kde4_add_library( karbonui SHARED ${karbonui_SRCS} )

target_link_libraries( karbonui ${KDE4_KDECORE_LIBS} 
                                karboncommon
                                komain
                                koguiutils
                                kotext
                                flake )

set_target_properties(karbonui PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS karbonui  DESTINATION ${LIB_INSTALL_DIR})

