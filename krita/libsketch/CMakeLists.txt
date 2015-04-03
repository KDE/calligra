add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(draganddrop)

set(kritasketchplugin_SRCS
    kritasketchplugin.cpp
    ColorSelectorItem.cpp
    CurveEditorItem.cpp
    Constants.cpp
    ToolManager.cpp
    SimpleTouchArea.cpp
    NetworkAccessManager.cpp
    ImageBuilder.cpp
    KritaNamespace.cpp
    MouseTracker.cpp
    PanelConfiguration.cpp
)

set(kritasketch_kritaintegrationmodels_SRCS
    models/TemplatesModel.cpp
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
    models/ColorModelModel.cpp
    models/ColorDepthModel.cpp
    models/ColorProfileModel.cpp
)


set(KRITA_COLORSELECTORNG_PART_SRCS
../plugins/extensions/dockers/advancedcolorselector/kis_my_paint_shade_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_patches.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_history.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_common_colors.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_common_colors_recalculation_runner.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_minimal_shade_selector.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_color_selector_base_proxy.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line_editor.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line_combo_box.cpp
../plugins/extensions/dockers/advancedcolorselector/kis_shade_selector_line_combo_box_popup.cpp
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

include(GNUInstallDirs)

# Sketch Qt Quick components
# =============================================================================

automoc4_add_library(kritasketchplugin MODULE ${kritasketchplugin_SRCS} ${kritasketch_kritaintegrationmodels_SRCS} ${KRITA_COLORSELECTORNG_PART_SRCS})

target_link_libraries(kritasketchplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_LIBRARIES}
    kritaui
    kowidgets
    ${KDE4_KFILE_LIBRARY}
    ${GLEW_LIBRARIES}
)

install(TARGETS   kritasketchplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/sketch)
install(FILES
                  qmldir
                  qml/HelpPage.qml
                  qml/KritaSketchBase.qml
                  qml/MainPage.qml
                  qml/OpenImagePage.qml
                  qml/SaveImagePage.qml
                  qml/SettingsPage.qml
                  qml/WelcomePage.qml
                  qml/CustomImagePage.qml
                                    DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/sketch)
install(DIRECTORY qml/components    DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/sketch PATTERN "*~" EXCLUDE)
install(DIRECTORY qml/images        DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/sketch PATTERN "*~" EXCLUDE)
install(DIRECTORY qml/panels        DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/sketch PATTERN "*~" EXCLUDE)

install(DIRECTORY themes DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
