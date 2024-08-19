add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_chart\")

# To hide all the warnings from embedded 3rd party software like kdchart
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-switch)
endif ()

add_subdirectory(icons)

if(BUILD_TESTING)
    add_subdirectory( tests )
endif()


include_directories( ${CMAKE_SOURCE_DIR}/plugins/chartshape
                     ${CMAKE_SOURCE_DIR}/plugins/chartshape/dialogs
                     ${CMAKE_SOURCE_DIR}/plugins/chartshape/widgets
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${KDCHART_INCLUDES}
                     ${KOMAIN_INCLUDES}
                     ${TEXTLAYOUT_INCLUDES}
)

########### Chart shape Plugin library ###############

set (chartshape_LIB_SRCS
    ChartDebug.cpp
    kochart_global.cpp
    KChartConvertions.cpp
    Axis.cpp
    DataSet.cpp
    CellRegion.cpp
    CellRegionStringValidator.cpp
    ChartTableModel.cpp
    Legend.cpp
    TextLabelDummy.cpp
    PlotArea.cpp
    TableSource.cpp
    ChartProxyModel.cpp
    KChartModel.cpp
    Surface.cpp
    ChartDocument.cpp
    ChartPart.cpp
    ChartShape.cpp
    ChartTool.cpp
    ChartToolFactory.cpp
    ConfigSubWidgetBase.cpp
    TitlesConfigWidget.cpp
    LegendConfigWidget.cpp
    PlotAreaConfigWidget.cpp
    AxesConfigWidget.cpp
    DataSetConfigWidget.cpp
    PieConfigWidget.cpp
    StockConfigWidget.cpp
    ChartTableView.cpp
    ScreenConversions.cpp
    ChartLayout.cpp
    SingleModelHelper.cpp
    OdfLoadingHelper.cpp
    OdfHelper.cpp
    dialogs/TableEditorDialog.cpp
    dialogs/NewAxisDialog.cpp
    dialogs/AxisScalingDialog.cpp
    dialogs/CellRegionDialog.cpp
    dialogs/FontEditorDialog.cpp
    dialogs/FormatErrorBarDialog.cpp
    dialogs/PieDataEditor.cpp
    dialogs/BubbleDataEditor.cpp
    dialogs/ScatterDataEditor.cpp
    dialogs/BubbleDataSetTableModel.cpp
    dialogs/ScatterDataSetTableModel.cpp
    dialogs/StockDataEditor.cpp
    commands/ChartTypeCommand.cpp
    commands/LegendCommand.cpp
    commands/AxisCommand.cpp
    commands/DatasetCommand.cpp
    commands/ChartTextShapeCommand.cpp
    commands/AddRemoveAxisCommand.cpp
    commands/GapCommand.cpp
    commands/PlotAreaCommand.cpp
    widgets/StrokeConfigWidget.cpp
    widgets/RadarDataSetConfigWidget.cpp
    widgets/RingConfigWidget.cpp
)

ki18n_wrap_ui(chartshape_LIB_SRCS
    StockConfigWidget.ui
    TitlesConfigWidget.ui
    LegendConfigWidget.ui
    PlotAreaConfigWidget.ui
    AxesConfigWidget.ui
    DataSetConfigWidget.ui
    PieConfigWidget.ui
    widgets/RadarDataSetConfigWidget.ui
    widgets/RingConfigWidget.ui
    dialogs/ChartTableEditor.ui
    dialogs/NewAxisDialog.ui
    dialogs/AxisScalingDialog.ui
    dialogs/CellRegionDialog.ui
    dialogs/FormatErrorBarDialog.ui
    dialogs/PieDataEditor.ui
    dialogs/BubbleDataEditor.ui
    dialogs/ScatterDataEditor.ui
    dialogs/StockDataEditor.ui
)

add_library(chartshapecore STATIC ${chartshape_LIB_SRCS})
target_link_libraries(chartshapecore  komain kotextlayout kowidgets KChart6)
if(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    set_target_properties(chartshapecore PROPERTIES COMPILE_FLAGS "-fPIC")
endif()


add_library(calligra_shape_chart MODULE ChartShapeFactory.cpp )

target_link_libraries(calligra_shape_chart chartshapecore KChart6 flake KF6::IconThemes)
install(TARGETS calligra_shape_chart DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
