/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin    <inge@lysator.liu.se>
   Copyright 2018 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Own
#include "DataSetConfigWidget.h"
#include "ui_DataSetConfigWidget.h"

// Qt
#include <QAction>
#include <QMenu>
#include <QLatin1String>

// KF5
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kfontchooser.h>

// Calligra
#include <interfaces/KoChartModel.h>
#include <KoIcon.h>

// KChart
#include <KChartChart>
#include <KChartPosition>
#include <KChartCartesianAxis>
#include <KChartGridAttributes>
#include <KChartAbstractCartesianDiagram>
#include <KChartLegend>
#include <KChartDataValueAttributes>
#include <KChartTextAttributes>
#include <KChartMarkerAttributes>
#include <KChartMeasure>

// KoChart
#include "ChartProxyModel.h"
#include "PlotArea.h"
#include "Legend.h"
#include "DataSet.h"
#include "Axis.h"
#include "ui_ChartTableEditor.h"
#include "NewAxisDialog.h"
#include "AxisScalingDialog.h"
#include "FontEditorDialog.h"
#include "FormatErrorBarDialog.h"
#include "CellRegionDialog.h"
#include "TableEditorDialog.h"
#include "commands/ChartTypeCommand.h"
#include "CellRegionStringValidator.h"
#include "ChartTableModel.h"
#include "TableSource.h"
#include "ChartDebug.h"

using namespace KoChart;


class DataSetConfigWidget::Private
{
public:
    Private(DataSetConfigWidget *parent);
    ~Private();

    Ui::DataSetConfigWidget ui;

    QList<Axis*> dataSetAxes;
    QList<DataSet*> dataSets;
    int selectedDataSet;

    // Menus
    QMenu *dataSetBarChartMenu;
    QMenu *dataSetLineChartMenu;
    QMenu *dataSetAreaChartMenu;
    QMenu *dataSetRadarChartMenu;
    QMenu *dataSetStockChartMenu;

    // chart type selection actions
    QAction  *normalBarChartAction;
    QAction  *stackedBarChartAction;
    QAction  *percentBarChartAction;

    QAction  *normalLineChartAction;
    QAction  *stackedLineChartAction;
    QAction  *percentLineChartAction;

    QAction  *normalAreaChartAction;
    QAction  *stackedAreaChartAction;
    QAction  *percentAreaChartAction;

    QAction  *circleChartAction;
    QAction  *ringChartAction;
    QAction  *radarChartAction;
    QAction  *filledRadarChartAction;

    QAction  *scatterChartAction;
    QAction  *bubbleChartAction;

    QAction  *hlcStockChartAction;
    QAction  *ohlcStockChartAction;
    QAction  *candlestickStockChartAction;

    QAction  *surfaceChartAction;
    QAction  *ganttChartAction;

    // chart type selection actions for datasets
    QAction  *dataSetNormalBarChartAction;
    QAction  *dataSetStackedBarChartAction;
    QAction  *dataSetPercentBarChartAction;

    QAction  *dataSetNormalLineChartAction;
    QAction  *dataSetStackedLineChartAction;
    QAction  *dataSetPercentLineChartAction;

    QAction  *dataSetNormalAreaChartAction;
    QAction  *dataSetStackedAreaChartAction;
    QAction  *dataSetPercentAreaChartAction;

    QAction  *dataSetCircleChartAction;
    QAction  *dataSetRingChartAction;
    QAction  *dataSetRadarChartAction;
    QAction  *dataSetFilledRadarChartAction;
    QAction  *dataSetScatterChartAction;
    QAction  *dataSetBubbleChartAction;

    QAction  *dataSetHLCStockChartAction;
    QAction  *dataSetOHLCStockChartAction;
    QAction  *dataSetCandlestickStockChartAction;

    QAction  *dataSetSurfaceChartAction;
    QAction  *dataSetGanttChartAction;

    // marker selection actions for datasets
    QAction *dataSetNoMarkerAction;
    QAction *dataSetAutomaticMarkerAction;
    QAction *dataSetMarkerCircleAction;
    QAction *dataSetMarkerSquareAction;
    QAction *dataSetMarkerDiamondAction;
    QAction *dataSetMarkerRingAction;
    QAction *dataSetMarkerCrossAction;
    QAction *dataSetMarkerFastCrossAction;
    QAction *dataSetMarkerArrowDownAction;
    QAction *dataSetMarkerArrowUpAction;
    QAction *dataSetMarkerArrowRightAction;
    QAction *dataSetMarkerArrowLeftAction;
    QAction *dataSetMarkerBowTieAction;
    QAction *dataSetMarkerHourGlassAction;
    QAction *dataSetMarkerStarAction;
    QAction *dataSetMarkerXAction;
    QAction *dataSetMarkerAsteriskAction;
    QAction *dataSetMarkerHorizontalBarAction;
    QAction *dataSetMarkerVerticalBarAction;

    FormatErrorBarDialog formatErrorBarDialog;
};


DataSetConfigWidget::Private::Private(DataSetConfigWidget *parent)
    : formatErrorBarDialog(parent)

{
    ui.setupUi(parent);

    selectedDataSet = 0;

    dataSetBarChartMenu = 0;
    dataSetLineChartMenu = 0;
    dataSetAreaChartMenu = 0;
    dataSetRadarChartMenu = 0;
    dataSetStockChartMenu = 0;
    dataSetNormalBarChartAction = 0;
    dataSetStackedBarChartAction = 0;
    dataSetPercentBarChartAction = 0;
    dataSetNormalLineChartAction = 0;
    dataSetStackedLineChartAction = 0;
    dataSetPercentLineChartAction = 0;
    dataSetNormalAreaChartAction = 0;
    dataSetStackedAreaChartAction = 0;
    dataSetPercentAreaChartAction = 0;
    dataSetCircleChartAction = 0;
    dataSetRingChartAction = 0;
    dataSetScatterChartAction = 0;
    dataSetRadarChartAction = 0;
    dataSetFilledRadarChartAction = 0;
    dataSetHLCStockChartAction = 0;
    dataSetOHLCStockChartAction = 0;
    dataSetCandlestickStockChartAction = 0;
    dataSetBubbleChartAction = 0;
    dataSetSurfaceChartAction = 0;
    dataSetGanttChartAction = 0;

    // Data set chart type button
    QMenu *dataSetChartTypeMenu = new QMenu(i18n("Chart Type"), parent);

    // Default chart type is a bar chart
    dataSetChartTypeMenu->setIcon(ICON1(BarChartType));


    dataSetBarChartMenu = dataSetChartTypeMenu->addMenu(ICON1(BarChartType), i18n("Bar Chart"));
    dataSetNormalBarChartAction  = dataSetBarChartMenu->addAction(ICON2(BarChartType, NormalChartSubtype), i18n("Normal"));
    dataSetStackedBarChartAction = dataSetBarChartMenu->addAction(ICON2(BarChartType, StackedChartSubtype), i18n("Stacked"));
    dataSetPercentBarChartAction = dataSetBarChartMenu->addAction(ICON2(BarChartType, PercentChartSubtype), i18n("Percent"));

    dataSetLineChartMenu = dataSetChartTypeMenu->addMenu(ICON1(LineChartType), i18n("Line Chart"));
    dataSetNormalLineChartAction  = dataSetLineChartMenu->addAction(ICON2(LineChartType, NormalChartSubtype), i18n("Normal"));
    dataSetStackedLineChartAction = dataSetLineChartMenu->addAction(ICON2(LineChartType, NormalChartSubtype), i18n("Stacked"));
    dataSetPercentLineChartAction = dataSetLineChartMenu->addAction(ICON2(LineChartType, NormalChartSubtype), i18n("Percent"));

    dataSetAreaChartMenu = dataSetChartTypeMenu->addMenu(ICON1(AreaChartType), i18n("Area Chart"));
    dataSetNormalAreaChartAction  = dataSetAreaChartMenu->addAction(ICON2(AreaChartType, NormalChartSubtype), i18n("Normal"));
    dataSetStackedAreaChartAction = dataSetAreaChartMenu->addAction(ICON2(AreaChartType, StackedChartSubtype), i18n("Stacked"));
    dataSetPercentAreaChartAction = dataSetAreaChartMenu->addAction(ICON2(AreaChartType, PercentChartSubtype), i18n("Percent"));

    dataSetCircleChartAction = dataSetChartTypeMenu->addAction(ICON1(CircleChartType), i18n("Pie Chart"));
    dataSetRingChartAction = dataSetChartTypeMenu->addAction(ICON1(RingChartType), i18n("Ring Chart"));

    dataSetRadarChartMenu = dataSetChartTypeMenu->addMenu(ICON1(RadarChartType), i18n("Polar Chart"));
    dataSetRadarChartAction = dataSetRadarChartMenu->addAction(ICON2(RadarChartType, NoChartSubtype), i18n("Normal"));
    dataSetFilledRadarChartAction = dataSetRadarChartMenu->addAction(ICON2(FilledRadarChartType, NoChartSubtype), i18n("Filled"));

    dataSetStockChartMenu = dataSetChartTypeMenu->addMenu(ICON1(StockChartType), i18n("Stock Chart"));
    dataSetCandlestickStockChartAction = dataSetStockChartMenu->addAction(ICON2(StockChartType, CandlestickChartSubtype), i18n("Candlestick"));
    dataSetOHLCStockChartAction = dataSetStockChartMenu->addAction(ICON2(StockChartType, OpenHighLowCloseChartSubtype), i18n("OpenHighLowClose"));
    dataSetHLCStockChartAction = dataSetStockChartMenu->addAction(ICON2(StockChartType, HighLowCloseChartSubtype), i18n("HighLowClose"));

    dataSetBubbleChartAction = dataSetChartTypeMenu->addAction(ICON1(BubbleChartType), i18n("Bubble Chart"));

    dataSetScatterChartAction = dataSetChartTypeMenu->addAction(ICON1(ScatterChartType), i18n("Scatter Chart"));

    ui.dataSetChartTypeMenu->setMenu(dataSetChartTypeMenu);

    connect(dataSetChartTypeMenu, SIGNAL(triggered(QAction*)), parent, SLOT(dataSetChartTypeSelected(QAction*)));

    connect(ui.dataSetHasChartType, SIGNAL(toggled(bool)), parent, SLOT(ui_dataSetHasChartTypeChanged(bool)));

    // Setup marker menu
    QMenu *datasetMarkerMenu = new QMenu(parent);

    // Default marker is Automatic
    datasetMarkerMenu->setIcon(QIcon());

    dataSetNoMarkerAction = datasetMarkerMenu->addAction(i18n("None"));
    dataSetAutomaticMarkerAction = datasetMarkerMenu->addAction(i18n("Automatic"));

    QMenu *datasetSelectMarkerMenu = datasetMarkerMenu->addMenu(i18n("Select"));
    dataSetMarkerSquareAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerDiamondAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerArrowDownAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerArrowUpAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerArrowRightAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerArrowLeftAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerBowTieAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerHourGlassAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerCircleAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerStarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerXAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerCrossAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerAsteriskAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerHorizontalBarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerVerticalBarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerRingAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    dataSetMarkerFastCrossAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());

    ui.datasetMarkerMenu->setMenu(datasetMarkerMenu);
    connect(datasetMarkerMenu, SIGNAL(triggered(QAction*)), parent, SLOT(datasetMarkerSelected(QAction*)));

    // Insert error bar button
    ui.formatErrorBar->setEnabled(false);

    connect(ui.datasetBrush, SIGNAL(changed(QColor)),parent, SLOT(datasetBrushSelected(QColor)));
    connect(ui.datasetPen, SIGNAL(changed(QColor)), parent, SLOT(datasetPenSelected(QColor)));
    connect(ui.datasetShowCategory, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowCategoryChanged(bool)));
    connect(ui.datasetShowErrorBar, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowErrorBarChanged(bool)));
    connect(ui.dataSetShowNumber, SIGNAL(toggled(bool)), parent, SLOT(ui_dataSetShowNumberChanged(bool)));
    connect(ui.datasetShowPercent, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowPercentChanged(bool)));

    // TODO
    // connect(ui.datasetShowSymbol, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowSymbolChanged(bool)));


    connect(ui.dataSets, SIGNAL(currentIndexChanged(int)), parent, SLOT(ui_dataSetSelectionChanged(int)));
    connect(ui.dataSetAxes, SIGNAL(currentIndexChanged(int)), parent, SLOT(ui_dataSetAxisSelectionChanged(int)));

}

DataSetConfigWidget::Private::~Private()
{
}


// ================================================================
//                     class DataSetConfigWidget


DataSetConfigWidget::DataSetConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
    , d(new Private(this))
{
    setObjectName("DataSetConfigWidget");

    setupDialogs();
    createActions();
}

DataSetConfigWidget::DataSetConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
    , d(new Private(this))
{
    setObjectName("DataSetConfigWidget");

    setupDialogs();
    createActions();
}

DataSetConfigWidget::~DataSetConfigWidget()
{
    delete d;
}

void DataSetConfigWidget::deleteSubDialogs(ChartType type)
{
    Q_UNUSED(type)
}

void DataSetConfigWidget::open(ChartShape* shape)
{
    debugChartUiDataSet<<shape;

    // reset these so we do not risc using old, deleted data
    // these are recalculated in updateData() anyway
    d->dataSetAxes.clear();
    d->dataSets.clear();

    ConfigSubWidgetBase::open(shape);
}

QAction *DataSetConfigWidget::createAction()
{
    return 0;
}

void DataSetConfigWidget::ui_dataSetAxisSelectionChanged(int index)
{
    if (index < 0 || d->ui.dataSets->currentIndex() >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<index<<d->dataSetAxes;
    if (index >= d->dataSetAxes.count()) {
        // Add the axis
        debugChartUiDataSet<<"create secondary y axis";
        emit axisAdded(YAxisDimension, i18n("Axistitle"));
    }
    if (index < 0 || index >= d->dataSetAxes.count()) {
        return;
    }
    DataSet *dataSet = d->dataSets[d->ui.dataSets->currentIndex()];
    Axis *axis = d->dataSetAxes[index];
    emit dataSetAxisChanged(dataSet, axis);
}

void DataSetConfigWidget::updateMarkers()
{
    DataSet *dataSet = d->dataSets[d->selectedDataSet];

    d->dataSetMarkerCircleAction->setIcon(dataSet->markerIcon(MarkerCircle));
    d->dataSetMarkerSquareAction->setIcon(dataSet->markerIcon(MarkerSquare));
    d->dataSetMarkerDiamondAction->setIcon(dataSet->markerIcon(MarkerDiamond));
    d->dataSetMarkerRingAction->setIcon(dataSet->markerIcon(MarkerRing));
    d->dataSetMarkerCrossAction->setIcon(dataSet->markerIcon(MarkerCross));
    d->dataSetMarkerFastCrossAction->setIcon(dataSet->markerIcon(MarkerFastCross));
    d->dataSetMarkerArrowDownAction->setIcon(dataSet->markerIcon(MarkerArrowDown));
    d->dataSetMarkerArrowUpAction->setIcon(dataSet->markerIcon(MarkerArrowUp));
    d->dataSetMarkerArrowRightAction->setIcon(dataSet->markerIcon(MarkerArrowRight));
    d->dataSetMarkerArrowLeftAction->setIcon(dataSet->markerIcon(MarkerArrowLeft));
    d->dataSetMarkerBowTieAction->setIcon(dataSet->markerIcon(MarkerBowTie));
    d->dataSetMarkerHourGlassAction->setIcon(dataSet->markerIcon(MarkerHourGlass));
    d->dataSetMarkerStarAction->setIcon(dataSet->markerIcon(MarkerStar));
    d->dataSetMarkerXAction->setIcon(dataSet->markerIcon(MarkerX));
    d->dataSetMarkerAsteriskAction->setIcon(dataSet->markerIcon(MarkerAsterisk));
    d->dataSetMarkerHorizontalBarAction->setIcon(dataSet->markerIcon(MarkerHorizontalBar));
    d->dataSetMarkerVerticalBarAction->setIcon(dataSet->markerIcon(MarkerVerticalBar));

    switch(dataSet->odfSymbolType()) {
        case NoSymbol:
            d->ui.datasetMarkerMenu->setText(i18n("None"));
            d->ui.datasetMarkerMenu->setIcon(QIcon());
            break;
        case NamedSymbol:
            d->ui.datasetMarkerMenu->setIcon(dataSet->markerIcon(dataSet->markerStyle()));
            d->ui.datasetMarkerMenu->setText(QString());
            break;
        case ImageSymbol:
        case AutomaticSymbol:
            d->ui.datasetMarkerMenu->setText(i18n("Auto"));
            d->ui.datasetMarkerMenu->setIcon(QIcon());
            break;
    }
}
void DataSetConfigWidget::ui_dataSetErrorBarTypeChanged()
{
    if (d->selectedDataSet < 0) {
        return;
    }
    debugChartUiDataSet;
    QString type = d->formatErrorBarDialog.widget.errorType->currentText();
    d->ui.formatErrorBar->setText(type);
}

void DataSetConfigWidget::ui_dataSetHasChartTypeChanged(bool b)
{
    if (d->selectedDataSet < 0) {
        return;
    }
    debugChartUiDataSet<<b;
    DataSet *dataSet = d->dataSets[d->selectedDataSet];
    Q_ASSERT(dataSet);
    if (!dataSet) {
        return;
    }
    if (b) {
        emit dataSetChartTypeChanged(dataSet, chart->chartType(), chart->chartSubType());
    } else {
        emit dataSetChartTypeChanged(dataSet, LastChartType, NoChartSubtype);
    }
}

void DataSetConfigWidget::dataSetChartTypeSelected(QAction *action)
{
    if (d->selectedDataSet < 0)
        return;

    debugChartUiDataSet<<action;

    ChartType     type    = LastChartType;
    ChartSubtype  subtype = NoChartSubtype;

    if (action == d->dataSetNormalBarChartAction) {
        type    = BarChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->dataSetStackedBarChartAction) {
        type    = BarChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->dataSetPercentBarChartAction) {
        type    = BarChartType;
        subtype = PercentChartSubtype;
    }

    else if (action == d->dataSetNormalLineChartAction) {
        type    = LineChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->dataSetStackedLineChartAction) {
        type    = LineChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->dataSetPercentLineChartAction) {
        type    = LineChartType;
        subtype = PercentChartSubtype;
    }

    else if (action == d->dataSetNormalAreaChartAction) {
        type    = AreaChartType;
        subtype = NormalChartSubtype;
    } else if (action == d->dataSetStackedAreaChartAction) {
        type    = AreaChartType;
        subtype = StackedChartSubtype;
    } else if (action == d->dataSetPercentAreaChartAction) {
        type    = AreaChartType;
        subtype = PercentChartSubtype;
    }

    else if (action == d->dataSetRadarChartAction)
        type = RadarChartType;
    else if (action == d->dataSetFilledRadarChartAction)
        type = FilledRadarChartType;

    else if (action == d->dataSetCircleChartAction)
        type = CircleChartType;
    else if (action == d->dataSetRingChartAction)
        type = RingChartType;
    else if (action == d->dataSetScatterChartAction)
        type = ScatterChartType;

    else if (action == d->dataSetHLCStockChartAction) {
        type    = StockChartType;
        subtype = HighLowCloseChartSubtype;
    } else if (action == d->dataSetStackedAreaChartAction) {
        type    = StockChartType;
        subtype = OpenHighLowCloseChartSubtype;
    } else if (action == d->dataSetCandlestickStockChartAction) {
        type    = StockChartType;
        subtype = CandlestickChartSubtype;
    }

    else if (action == d->dataSetBubbleChartAction)
        type = BubbleChartType;

    // FIXME: Not supported by KoChart yet:
    //surface
    //gantt

    DataSet *dataSet = d->dataSets[d->selectedDataSet];
    Q_ASSERT(dataSet);
    if (!dataSet)
        return;

    const QLatin1String iconName = chartTypeIconName(type, subtype);
    if (iconName.size() > 0)
        d->ui.dataSetChartTypeMenu->setIcon(QIcon::fromTheme(iconName));

    emit dataSetChartTypeChanged(dataSet, type, subtype);

    updateData(type, subtype);
}

void DataSetConfigWidget::datasetMarkerSelected(QAction *action)
{
    if (d->selectedDataSet < 0)
        return;

    const int numDefaultMarkerTypes = 15;
    OdfMarkerStyle style = MarkerSquare;
    QString type = QString("");
    OdfSymbolType symbolType = NamedSymbol;
    if (action == d->dataSetNoMarkerAction) {
        symbolType = NoSymbol;
        type = "None";
    } else if (action == d->dataSetAutomaticMarkerAction) {
        style = (OdfMarkerStyle) (d->selectedDataSet % numDefaultMarkerTypes);
        type = "Auto";
        symbolType = AutomaticSymbol;
    } else if (action == d->dataSetMarkerCircleAction) {
        style = MarkerCircle;
    } else if (action == d->dataSetMarkerSquareAction) {
        style = MarkerSquare;
    } else if (action == d->dataSetMarkerDiamondAction) {
        style = MarkerDiamond;
    } else if (action == d->dataSetMarkerRingAction) {
        style = MarkerRing;
    } else if (action == d->dataSetMarkerCrossAction) {
        style = MarkerCross;
    } else if (action == d->dataSetMarkerFastCrossAction) {
        style = MarkerFastCross;
    } else if (action == d->dataSetMarkerArrowDownAction) {
        style = MarkerArrowDown;
    } else if (action == d->dataSetMarkerArrowUpAction) {
        style = MarkerArrowUp;
    } else if (action == d->dataSetMarkerArrowRightAction) {
        style = MarkerArrowRight;
    } else if (action == d->dataSetMarkerArrowLeftAction) {
        style = MarkerArrowLeft;
    } else if (action == d->dataSetMarkerBowTieAction) {
        style = MarkerBowTie;
    } else if (action == d->dataSetMarkerHourGlassAction) {
        style = MarkerHourGlass;
    } else if (action == d->dataSetMarkerStarAction) {
        style = MarkerStar;
    } else if (action == d->dataSetMarkerXAction) {
        style = MarkerX;
    } else if (action == d->dataSetMarkerAsteriskAction) {
        style = MarkerAsterisk;
    } else if (action == d->dataSetMarkerHorizontalBarAction) {
        style = MarkerHorizontalBar;
    } else if (action == d->dataSetMarkerVerticalBarAction) {
        style = MarkerVerticalBar;
    }

    DataSet *dataSet = d->dataSets[d->selectedDataSet];
    Q_ASSERT(dataSet);
    if (!dataSet)
        return;

    if (type.isEmpty()) {
        d->ui.datasetMarkerMenu->setIcon(dataSet->markerIcon(style));
        d->ui.datasetMarkerMenu->setText("");
    } else {
        d->ui.datasetMarkerMenu->setText(type);
        d->ui.datasetMarkerMenu->setIcon(QIcon());
    }
    emit dataSetMarkerChanged(dataSet, symbolType, style);

    updateData(dataSet->chartType(), dataSet->chartSubType());
}

void DataSetConfigWidget::datasetBrushSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetBrushChanged(d->dataSets[d->selectedDataSet], color, -1);
    updateMarkers();
}

void DataSetConfigWidget::datasetPenSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetPenChanged(d->dataSets[d->selectedDataSet], color, -1);
    updateMarkers();
}

void DataSetConfigWidget::updateData(ChartType chartType, ChartSubtype subtype)
{
    Q_UNUSED(subtype)
    if (!chart) {
        return;
    }
    if (!chartTypes.contains(chartType)) {
        return;
    }
    QList<DataSet*> newDataSets = chart->plotArea()->dataSets();
    debugChartUiDataSet<<d->selectedDataSet<<d->dataSets<<':'<<newDataSets;

    blockSignals(true);
    d->dataSetAxes.clear();
    d->ui.dataSets->clear();

    // Update the chart type specific settings in the "Data Sets" tab
    d->ui.errorBarProperties->hide();
    // TODO: error indication is not implemented
//     if (chartType == BarChartType || chartType == LineChartType || chartType == AreaChartType || chartType == ScatterChartType) {
//         d->ui.errorBarProperties->show();
//     }
    // TODO 
//     d->ui.datasetShowSymbol->hide();
//     if (chartType == LineChartType || chartType == AreaChartType || chartType == ScatterChartType || chartType == RadarChartType || chartType == FilledRadarChartType) {
//         d->ui.datasetShowSymbol->show();
//     }
    
    // Make sure we only allow legal chart type combinations
    if (isPolar(chartType)) {
        // TODO: check out this claim:
        // Pie charts and ring charts have no axes but radar charts do.
        // Disable choosing of attached axis if there is none.
        bool hasAxes = !(chartType == CircleChartType || chartType == RingChartType);
//         d->ui.axisConfiguration->setEnabled(hasAxes);
        d->ui.dataSetAxes->setCurrentIndex(-1);
        d->ui.dataSetAxes->setEnabled(false);
        d->ui.dataSetHasChartType->setEnabled(hasAxes);
        d->ui.dataSetChartTypeMenu->setEnabled(hasAxes);
    } else {
        // All the cartesian chart types have axes.
//         d->ui.axisConfiguration->setEnabled(true);
        d->dataSetAxes.append(chart->plotArea()->yAxis());
        if (chart->plotArea()->secondaryYAxis() && chart->plotArea()->secondaryYAxis()->isVisible()) {
            d->dataSetAxes.append(chart->plotArea()->secondaryYAxis());
        }
        d->ui.dataSetAxes->setEnabled(true);
        d->ui.dataSetHasChartType->setEnabled(true);
        d->ui.dataSetChartTypeMenu->setEnabled(true);
    }

    // always update, in case titles have changed
    foreach (DataSet *dataSet, newDataSets) {
        QString title = dataSet->labelData().toString();
        if (title.isEmpty())
            title = i18n("Data Set %1", d->ui.dataSets->count() + 1);
        d->ui.dataSets->addItem(title);
    }
    if (newDataSets != d->dataSets) {
        d->selectedDataSet = 0; // new datasets, select the first
        d->dataSets = newDataSets;
        debugChartUiDataSet<<"new datasets"<<newDataSets;
    }
    d->ui.dataSets->setCurrentIndex(d->selectedDataSet);

    blockSignals(false);

    ui_dataSetSelectionChanged(d->selectedDataSet);
}


void DataSetConfigWidget::slotShowFormatErrorBarDialog()
{
    d->formatErrorBarDialog.show();
}

void DataSetConfigWidget::setupDialogs()
{
    // Format Error Bars
    connect(d->ui.formatErrorBar, SIGNAL(clicked()),
             this, SLOT(slotShowFormatErrorBarDialog()));
    connect(&d->formatErrorBarDialog, SIGNAL(accepted()),
             this, SLOT(ui_dataSetErrorBarTypeChanged()));
}

void DataSetConfigWidget::createActions()
{
}

void DataSetConfigWidget::ui_dataSetSelectionChanged(int index)
{
    // Check for valid index
    debugChartUiDataSet<<index<<d->dataSets;
    if (index < 0 || index >= d->dataSets.size()) {
        return;
    }
    blockSignals(true);

    DataSet *dataSet = d->dataSets[index];
    //d->ui.datasetColor->setText(axis->titleText());
    d->ui.dataSetAxes->setCurrentIndex(d->dataSetAxes.indexOf(dataSet->attachedAxis()));

    d->ui.datasetBrush->setColor(dataSet->brush().color());

    d->ui.datasetPen->setColor(dataSet->pen().color());

    debugChartUiDataSet<<dataSet->valueLabelType();
    d->ui.datasetShowCategory->setChecked(dataSet->valueLabelType().category);

    d->ui.dataSetShowNumber->setChecked(dataSet->valueLabelType().number);

    d->ui.datasetShowPercent->setChecked(dataSet->valueLabelType().percentage);

    // TODO
    // d->ui.datasetShowSymbol->setChecked(dataSet->valueLabelType().symbol);

    d->ui.dataSetHasChartType->setChecked(dataSet->chartType() != LastChartType);
    d->ui.dataSetChartTypeMenu->setEnabled(dataSet->chartType() != LastChartType);

    Q_ASSERT(d->ui.dataSetChartTypeMenu->menu());

    d->ui.dataSetChartTypeMenu->setIcon(ICON2(dataSet->chartType(), dataSet->chartSubType()));

    d->selectedDataSet = index;

    blockSignals(false);

    updateMarkers();

    ChartType chartType = dataSet->chartType();
    if (chartType == LastChartType) {
        chartType = chart->chartType();
    }
    bool disableMarkers = chartType == BarChartType || chartType == StockChartType || chartType == CircleChartType
                            || chartType == RingChartType || chartType == BubbleChartType;
    d->ui.datasetMarkerMenu->setDisabled(disableMarkers);
}

void DataSetConfigWidget::ui_datasetShowCategoryChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowCategoryChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void DataSetConfigWidget::ui_datasetShowErrorBarChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    d->ui.formatErrorBar->setEnabled(b);
}

void DataSetConfigWidget::ui_dataSetShowNumberChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit dataSetShowNumberChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void DataSetConfigWidget::ui_datasetShowPercentChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowPercentChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void DataSetConfigWidget::ui_datasetShowSymbolChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowSymbolChanged(d->dataSets[d->selectedDataSet], b, -1);
}
