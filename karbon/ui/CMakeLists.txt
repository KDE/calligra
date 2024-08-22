include_directories(
        ${KOPAGEAPP_INCLUDES}
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
    KarbonUiDebug.cpp
    KarbonFactory.cpp
    KarbonDocument.cpp
    KarbonPart.cpp
    KarbonView.cpp 
    ProxyView.cpp

    KarbonDocumentMergeCommand.cpp
    
    dialogs/KarbonConfigureDialog.cpp

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
        kopageapp
        kowidgets
        koodf
        koplugin
        KF6::IconThemes
        KF6::GuiAddons
)

set_target_properties(karbonui PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )

install(TARGETS karbonui  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

