add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${KRITA_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(draganddrop)

set(kritasketch_SRCS
    ColorSelectorItem.cpp
    Constants.cpp
    DocumentListModel.cpp
    IconImageProvider.cpp
    KisSketchCanvas.cpp
    KisSketchCanvasFactory.cpp
    KisSketchView.cpp
    main.cpp
    MainWindow.cpp
    ProgressProxy.cpp
    RecentFileManager.cpp
    Settings.cpp
    NetworkAccessManager.cpp
)

set(kritasketch_kritaintegrationmodels_SRCS
    models/ColorImageProvider.cpp
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

kde4_add_executable(kritasketch ${kritasketch_SRCS} ${kritasketch_kritaintegrationmodels_SRCS} ${KRITA_COLORSELECTORNG_PART_SRCS})

target_link_libraries(kritasketch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
    ${GLEW_LIBRARY}
    ${KDE4_KFILE_LIBRARY}
)

install(TARGETS kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritasketch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritasketch PATTERN "*~" EXCLUDE)
install(DIRECTORY fonts DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
install( FILES kritasketchrc DESTINATION ${CONFIG_INSTALL_DIR} )
