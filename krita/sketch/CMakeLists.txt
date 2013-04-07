if(WIN32)
    option(ENABLE_SKETCH_STARTUP_MAGIC "Enable Sketch's windows startup magic" ON)

    if(ENABLE_SKETCH_STARTUP_MAGIC)
        add_definitions(-DSKETCH_ENABLE_STARTUP_MAGIC)
    endif(ENABLE_SKETCH_STARTUP_MAGIC)
endif(WIN32)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${KRITA_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(draganddrop)
add_subdirectory(paintoppresets)

set(kritasketch_SRCS
    cpuid.cpp
    ColorSelectorItem.cpp
    Constants.cpp
    DocumentListModel.cpp
    KisSketchView.cpp
    KisSketchPart.cpp
    KisSelectionExtras.cpp
    ToolManager.cpp
    main.cpp
    MainWindow.cpp
    ProgressProxy.cpp
    RecentFileManager.cpp
    Settings.cpp
    SimpleTouchArea.cpp
    NetworkAccessManager.cpp
    ImageBuilder.cpp
    KritaNamespace.cpp
    MouseTracker.cpp
    PanelConfiguration.cpp
    PropertyContainer.cpp
    SketchInputContext.cpp
    VirtualKeyboardController.cpp
    DocumentManager.cpp
    SharingManager.cpp
)

if(HAVE_OPENGL)
    set(kritasketch_SRCS
        ${kritasketch_SRCS}
        KisSketchCanvas.cpp
        KisSketchCanvasFactory.cpp
    )
endif(HAVE_OPENGL)

set(kritasketch_kritaintegrationmodels_SRCS
    models/KeyboardModel.cpp
    models/ColorImageProvider.cpp
    models/CompositeOpModel.cpp
    models/FiltersCategoryModel.cpp
    models/FiltersModel.cpp
    models/LayerCompositeDetails.cpp
    models/LayerModel.cpp
    models/LayerThumbProvider.cpp
    models/PaletteColorsModel.cpp
    models/PaletteModel.cpp
    models/PresetImageProvider.cpp
    models/PresetModel.cpp
    models/RecentImageImageProvider.cpp
    models/RecentImagesModel.cpp
    models/MultiFeedRSSModel.cpp
    models/FileSystemModel.cpp
)


set(KRITA_COLORSELECTORNG_PART_SRCS
../plugins/extensions/dockers/advancedcolorselector/kis_my_paint_shade_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_patches.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_history.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_common_colors.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_common_colors_recalculation_runner.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_minimal_shade_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line_combo_box.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_lines_settings.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_container.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_base.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_component.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_ring.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_triangle.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_simple.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_wheel.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_combo_box.cpp
)


if (WIN32)
    set(kritasketch_SRCS ${kritasketch_SRCS} KritaSketch.rc)
else (WIN32)
    kde4_add_app_icon(kritasketch_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kritasketch.png")
endif (WIN32)

kde4_add_executable(kritasketch ${kritasketch_SRCS} ${kritasketch_kritaintegrationmodels_SRCS} ${KRITA_COLORSELECTORNG_PART_SRCS})


target_link_libraries(kritasketch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
    ${KDE4_KFILE_LIBRARY}
)
if(HAVE_OPENGL)
    target_link_libraries(kritasketch
        ${GLEW_LIBRARY}
    )
endif(HAVE_OPENGL)

install(TARGETS kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritasketch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritasketch PATTERN "*~" EXCLUDE)
install(DIRECTORY fonts DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
install( FILES kritasketchrc kritasketchpanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
