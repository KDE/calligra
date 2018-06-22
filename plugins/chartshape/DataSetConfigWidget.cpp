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
#include <KChartPieAttributes>
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
    Private(QWidget *parent);
    ~Private();

    ChartShape *shape;

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


DataSetConfigWidget::Private::Private(QWidget *parent)
    : formatErrorBarDialog(parent)

{
    shape = 0;
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
}

DataSetConfigWidget::Private::~Private()
{
}


// ================================================================
//                     class DataSetConfigWidget

/**
 * Returns, if existent, the name of the icon representing
 * a given chart type, following the KDE4 icon naming convention.
 */
static const char * chartTypeIconName(ChartType type, ChartSubtype subtype)
{
    switch(type) {
    case BarChartType:
        switch(subtype) {
        case NormalChartSubtype:
            return koIconNameCStr("office-chart-bar");
        case StackedChartSubtype:
            return koIconNameCStr("office-chart-bar-stacked");
        case PercentChartSubtype:
            return koIconNameCStr("office-chart-bar-percentage");
        default:
            Q_ASSERT("Invalid bar chart subtype!");
        }
    case LineChartType:
        switch(subtype) {
        case NormalChartSubtype:
            return koIconNameCStr("office-chart-line");
        case StackedChartSubtype:
            return koIconNameCStr("office-chart-line-stacked");
        case PercentChartSubtype:
            return koIconNameCStr("office-chart-line-percentage");
        default:
            Q_ASSERT("Invalid line chart subtype!");
        }
    case AreaChartType:
        switch(subtype) {
        case NormalChartSubtype:
            return koIconNameCStr("office-chart-area");
        case StackedChartSubtype:
            return koIconNameCStr("office-chart-area-stacked");
        case PercentChartSubtype:
            return koIconNameCStr("office-chart-area-percentage");
        default:
            Q_ASSERT("Invalid area chart subtype!");
        }
    case CircleChartType:
        return koIconNameCStr("office-chart-pie");
    case RingChartType:
        return koIconNameCStr("office-chart-ring");
    case RadarChartType:
        return koIconNameCStr("office-chart-polar");
    case FilledRadarChartType:
        return koIconNameCStr("office-chart-polar-filled");
    case ScatterChartType:
        return koIconNameCStr("office-chart-scatter");
    default:
        return "";
    }

    return "";
}

DataSetConfigWidget::DataSetConfigWidget()
    : d(new Private(this))
{
    setObjectName("DataSetConfigWidget");
    d->ui.setupUi(this);

    // Data set chart type button
    QMenu *dataSetChartTypeMenu = new QMenu(this);

    // Default chart type is a bar chart
    dataSetChartTypeMenu->setIcon(koIcon("office-chart-bar"));


    d->dataSetBarChartMenu = dataSetChartTypeMenu->addMenu(koIcon("office-chart-bar"), "Bar Chart");
    d->dataSetNormalBarChartAction  = d->dataSetBarChartMenu->addAction(koIcon("office-chart-bar"), i18n("Normal"));
    d->dataSetStackedBarChartAction = d->dataSetBarChartMenu->addAction(koIcon("office-chart-bar-stacked"), i18n("Stacked"));
    d->dataSetPercentBarChartAction = d->dataSetBarChartMenu->addAction(koIcon("office-chart-bar-percentage"), i18n("Percent"));

    d->dataSetLineChartMenu = dataSetChartTypeMenu->addMenu(koIcon("office-chart-line"), "Line Chart");
    d->dataSetNormalLineChartAction  = d->dataSetLineChartMenu->addAction(koIcon("office-chart-line"), i18n("Normal"));
    d->dataSetStackedLineChartAction = d->dataSetLineChartMenu->addAction(koIcon("office-chart-line-stacked"), i18n("Stacked"));
    d->dataSetPercentLineChartAction = d->dataSetLineChartMenu->addAction(koIcon("office-chart-line-percentage"), i18n("Percent"));

    d->dataSetAreaChartMenu = dataSetChartTypeMenu->addMenu(koIcon("office-chart-area"), "Area Chart");
    d->dataSetNormalAreaChartAction  = d->dataSetAreaChartMenu->addAction(koIcon("office-chart-area"), i18n("Normal"));
    d->dataSetStackedAreaChartAction = d->dataSetAreaChartMenu->addAction(koIcon("office-chart-area-stacked"), i18n("Stacked"));
    d->dataSetPercentAreaChartAction = d->dataSetAreaChartMenu->addAction(koIcon("office-chart-area-percentage"), i18n("Percent"));

    d->dataSetCircleChartAction = dataSetChartTypeMenu->addAction(koIcon("office-chart-pie"), i18n("Pie Chart"));
    d->dataSetRingChartAction = dataSetChartTypeMenu->addAction(koIcon("office-chart-ring"), i18n("Ring Chart"));

    d->dataSetRadarChartMenu = dataSetChartTypeMenu->addMenu(koIcon("office-chart-polar"), "Polar Chart");
    d->dataSetRadarChartAction = d->dataSetRadarChartMenu->addAction(koIcon("office-chart-polar"), i18n("Normal"));
    d->dataSetFilledRadarChartAction = d->dataSetRadarChartMenu->addAction(koIcon("office-chart-polar-filled"), i18n("Filled"));

    d->dataSetStockChartMenu = dataSetChartTypeMenu->addMenu("Stock Chart");
    d->dataSetHLCStockChartAction = d->dataSetStockChartMenu->addAction(i18n("HighLowClose"));
    d->dataSetOHLCStockChartAction = d->dataSetStockChartMenu->addAction(i18n("OpenHighLowClose"));
    d->dataSetCandlestickStockChartAction = d->dataSetStockChartMenu->addAction(i18n("Candlestick"));

    d->dataSetBubbleChartAction = dataSetChartTypeMenu->addAction(i18n("Bubble Chart"));

    d->dataSetScatterChartAction = dataSetChartTypeMenu->addAction(koIcon("office-chart-scatter"), i18n("Scatter Chart"));

    d->ui.dataSetChartTypeMenu->setMenu(dataSetChartTypeMenu);

    connect(dataSetChartTypeMenu, SIGNAL(triggered(QAction*)),
            this,                 SLOT(dataSetChartTypeSelected(QAction*)));

    connect(d->ui.dataSetHasChartType, SIGNAL(toggled(bool)),
            this,                      SLOT(ui_dataSetHasChartTypeChanged(bool)));

    // Setup marker menu
    QMenu *datasetMarkerMenu = new QMenu(this);

    // Default marker is Automatic
    datasetMarkerMenu->setIcon(QIcon());

    d->dataSetNoMarkerAction = datasetMarkerMenu->addAction(i18n("None"));
    d->dataSetAutomaticMarkerAction = datasetMarkerMenu->addAction(i18n("Automatic"));

    QMenu *datasetSelectMarkerMenu = datasetMarkerMenu->addMenu(i18n("Select"));
    d->dataSetMarkerSquareAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerDiamondAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerArrowDownAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerArrowUpAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerArrowRightAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerArrowLeftAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerBowTieAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerHourGlassAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerCircleAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerStarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerXAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerCrossAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerAsteriskAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerHorizontalBarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerVerticalBarAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerRingAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());
    d->dataSetMarkerFastCrossAction = datasetSelectMarkerMenu->addAction(QIcon(), QString());

    d->ui.datasetMarkerMenu->setMenu(datasetMarkerMenu);
    connect(datasetMarkerMenu, SIGNAL(triggered(QAction*)),
            this,              SLOT(datasetMarkerSelected(QAction*)));

    // Insert error bar button
    d->ui.formatErrorBar->setEnabled(false);

    connect(d->ui.datasetBrush, SIGNAL(changed(QColor)),
            this, SLOT(datasetBrushSelected(QColor)));
    connect(d->ui.datasetPen, SIGNAL(changed(QColor)),
            this, SLOT(datasetPenSelected(QColor)));
    connect(d->ui.datasetShowCategory, SIGNAL(toggled(bool)),
            this, SLOT(ui_datasetShowCategoryChanged(bool)));
    connect(d->ui.datasetShowErrorBar, SIGNAL(toggled(bool)),
            this, SLOT(ui_datasetShowErrorBarChanged(bool)));
    connect(d->ui.dataSetShowNumber, SIGNAL(toggled(bool)),
            this, SLOT(ui_dataSetShowNumberChanged(bool)));
    connect(d->ui.datasetShowPercent, SIGNAL(toggled(bool)),
            this, SLOT(ui_datasetShowPercentChanged(bool)));
    connect(d->ui.datasetShowSymbol, SIGNAL(toggled(bool)),
            this, SLOT(ui_datasetShowSymbolChanged(bool)));
    connect(d->ui.pieExplodeFactor, SIGNAL(valueChanged(int)),
            this, SLOT(ui_dataSetPieExplodeFactorChanged(int)));


    connect(d->ui.dataSets, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ui_dataSetSelectionChanged(int)));
    connect(d->ui.dataSetAxes, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ui_dataSetAxisSelectionChanged(int)));

    setupDialogs();
    createActions();
}

DataSetConfigWidget::~DataSetConfigWidget()
{
    delete d;
}

void DataSetConfigWidget::deleteSubDialogs()
{
}

void DataSetConfigWidget::open(KoShape* shape)
{
    debugChartUiDataSet<<shape;

    // reset these so we do not risc using old, deleted data
    d->shape = 0;
    // these are recalculated in updateData() anyway
    d->dataSetAxes.clear();
    d->dataSets.clear();

    d->shape = dynamic_cast<ChartShape*>(shape);
    if (!d->shape) {
        d->shape = dynamic_cast<ChartShape*>(shape->parent());
        if (!d->shape) {
            return;
        }
    }
    updateData();
}

QAction * DataSetConfigWidget::createAction()
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
        emit axisAdded(YAxisDimension, "TITLE");
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

    OdfMarkerStyle style = dataSet->markerStyle();
    QIcon icon = dataSet->markerIcon(style);
    if (!icon.isNull()) {
        if (dataSet->markerAutoSet()) {
            d->ui.datasetMarkerMenu->setText(i18n("Auto"));
            d->ui.datasetMarkerMenu->setIcon(QIcon());
        } else {
            d->ui.datasetMarkerMenu->setIcon(icon);
            d->ui.datasetMarkerMenu->setText(QString());
        }
    } else {
        d->ui.datasetMarkerMenu->setText(i18n("None"));
        d->ui.datasetMarkerMenu->setIcon(QIcon());
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

void DataSetConfigWidget::ui_dataSetPieExplodeFactorChanged(int percent)
{
    if (d->selectedDataSet < 0) {
        return;
    }
    debugChartUiDataSet<<percent;
    DataSet *dataSet = d->dataSets[d->selectedDataSet];
    Q_ASSERT(dataSet);
    if (!dataSet)
        return;

    emit pieExplodeFactorChanged(dataSet, percent);
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
    emit dataSetChartTypeChanged(dataSet, b ? d->shape->chartType() : LastChartType);
    emit dataSetChartSubTypeChanged(dataSet, b ? d->shape->chartSubType() : NoChartSubtype);
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
    } else if (action == d->dataSetPercentAreaChartAction) {
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

    const QString iconName = QLatin1String(chartTypeIconName(type, subtype));
    if (!iconName.isEmpty())
        d->ui.dataSetChartTypeMenu->setIcon(QIcon::fromTheme(iconName));

    emit dataSetChartTypeChanged(dataSet, type);
    emit dataSetChartSubTypeChanged(dataSet, subtype);

    updateData();
}

void DataSetConfigWidget::datasetMarkerSelected(QAction *action)
{
    if (d->selectedDataSet < 0)
        return;

    const int numDefaultMarkerTypes = 15;
    bool isAuto = false;
    OdfMarkerStyle style = MarkerSquare;
    QString type = QString("");
    if (action == d->dataSetNoMarkerAction) {
        style = NoMarker;
        type = "None";
    } else if (action == d->dataSetAutomaticMarkerAction) {
        style = (OdfMarkerStyle) (d->selectedDataSet % numDefaultMarkerTypes);
        type = "Auto";
        isAuto = true;
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

    dataSet->setAutoMarker(isAuto);
    if (type.isEmpty()) {
        d->ui.datasetMarkerMenu->setIcon(dataSet->markerIcon(style));
        d->ui.datasetMarkerMenu->setText("");
    } else {
        d->ui.datasetMarkerMenu->setText(type);
        d->ui.datasetMarkerMenu->setIcon(QIcon());
    }
    emit dataSetMarkerChanged(dataSet, style);

    updateData();
}

void DataSetConfigWidget::datasetBrushSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetBrushChanged(d->dataSets[d->selectedDataSet], color);
    updateMarkers();
}

void DataSetConfigWidget::datasetPenSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetPenChanged(d->dataSets[d->selectedDataSet], color);
    updateMarkers();
}

void DataSetConfigWidget::updateData()
{
    if (!d->shape) {
        return;
    }
    blockSignals(true);
    d->dataSetAxes.clear();
    d->ui.dataSets->clear();
    // This is used in a couple of places.
    QList<DataSet*> newDataSets = d->shape->plotArea()->dataSets();
    debugChartUiDataSet<<d->selectedDataSet<<d->dataSets<<':'<<newDataSets;

    // Update "Pie Properties" in "Data Sets" tab
    if (!newDataSets.isEmpty()) {
        d->ui.pieExplodeFactor->setValue((int)(newDataSets.at(0)->pieAttributes().explodeFactor()*100));
    }

    ChartType chartType = d->shape->chartType();

    // Update the chart type specific settings in the "Data Sets" tab
    if (chartType == BarChartType) {
        d->ui.pieProperties->hide();
        d->ui.errorBarProperties->show();
    } else if (chartType == LineChartType || chartType == AreaChartType
                || chartType == ScatterChartType) {
        d->ui.pieProperties->hide();
        d->ui.errorBarProperties->show();
    } else if (chartType == CircleChartType || chartType == RingChartType) {
        d->ui.pieProperties->show();
        d->ui.errorBarProperties->hide();
    } else {
        d->ui.pieProperties->hide();
        d->ui.errorBarProperties->hide();
    }

    // Make sure we only allow legal chart type combinations
    if (isPolar(chartType)) {
        setPolarChartTypesEnabled(true);
        setCartesianChartTypesEnabled(false);

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
        setPolarChartTypesEnabled(false);
        setCartesianChartTypesEnabled(true);

        // All the cartesian chart types have axes.
//         d->ui.axisConfiguration->setEnabled(true);
        d->dataSetAxes.append(d->shape->plotArea()->yAxis());
        if (d->shape->plotArea()->secondaryYAxis() && d->shape->plotArea()->secondaryYAxis()->isVisible()) {
            d->dataSetAxes.append(d->shape->plotArea()->secondaryYAxis());
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

    bool enableMarkers = !(chartType == BarChartType || chartType == StockChartType || chartType == CircleChartType
                            || chartType == RingChartType || chartType == BubbleChartType);
    d->ui.datasetMarkerMenu->setEnabled(enableMarkers);

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

    d->ui.datasetShowCategory->setChecked(dataSet->valueLabelType().category);

    d->ui.dataSetShowNumber->setChecked(dataSet->valueLabelType().number);

    d->ui.datasetShowPercent->setChecked(dataSet->valueLabelType().percentage);

    d->ui.datasetShowSymbol->setChecked(dataSet->valueLabelType().symbol);

    d->ui.dataSetHasChartType->setChecked(dataSet->chartType() != LastChartType);
    d->ui.dataSetChartTypeMenu->setEnabled(dataSet->chartType() != LastChartType);

    Q_ASSERT(d->ui.dataSetChartTypeMenu->menu());

    if (dataSet->chartType() == LastChartType) {
        d->ui.dataSetChartTypeMenu->setIcon(QIcon());
    } else {
        switch (dataSet->chartType()) {
        case BarChartType:
            switch (dataSet->chartSubType()) {
            case StackedChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-bar-stacked"));
                break;
            case PercentChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-bar-percentage"));
                break;
            default:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-bar"));
            }
            break;
        case LineChartType:
            switch (dataSet->chartSubType()) {
            case StackedChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-line-stacked"));
                break;
            case PercentChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-line-percentage"));
                break;
            default:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-line"));
            }
            break;
        case AreaChartType:
            switch (dataSet->chartSubType()) {
            case StackedChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-area-stacked"));
                break;
            case PercentChartSubtype:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-area-percentage"));
                break;
            default:
                d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-area"));
            }
            break;
        case CircleChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-pie"));
            break;
        case RingChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-ring"));
            break;
        case ScatterChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-scatter"));
            break;
        case RadarChartType:
            d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-polar"));
            break;
        case FilledRadarChartType:
            d->ui.dataSetChartTypeMenu->setIcon(koIcon("office-chart-polar-filled"));
            break;
        case StockChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-stock"));
            break;
        case BubbleChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-bubble"));
            break;
        case SurfaceChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-surface"));
            break;
        case GanttChartType:
            d->ui.dataSetChartTypeMenu->menu()->setIcon(koIcon("office-chart-gantt"));
            break;

            // Fixes a warning that LastChartType isn't handled.
        default:
            break;
        }
    }
    d->selectedDataSet = index;

    blockSignals(false);

    updateMarkers();
}

/**
 * Enabled/Disabled menu actions to set a polar chart type
 */
void DataSetConfigWidget::setPolarChartTypesEnabled(bool enabled)
{
    d->dataSetCircleChartAction->setEnabled(enabled);
    d->dataSetRingChartAction->setEnabled(enabled);
    d->dataSetRadarChartAction->setEnabled(enabled);
    d->dataSetFilledRadarChartAction->setEnabled(enabled);
}

/**
 * Enabled/Disabled menu actions to set a cartesian chart type
 */
void DataSetConfigWidget::setCartesianChartTypesEnabled(bool enabled)
{
    d->dataSetBarChartMenu->setEnabled(enabled);
    d->dataSetLineChartMenu->setEnabled(enabled);
    d->dataSetAreaChartMenu->setEnabled(enabled);
    d->dataSetRadarChartMenu->setEnabled(enabled);
    d->dataSetScatterChartAction->setEnabled(enabled);
    d->dataSetStockChartMenu->setEnabled(enabled);
    d->dataSetBubbleChartAction->setEnabled(enabled);
    // FIXME: Enable for:
    // pie, ring?
    //NYI:
    //surface
    //gantt
}

void DataSetConfigWidget::ui_datasetShowCategoryChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowCategoryChanged(d->dataSets[d->selectedDataSet], b);
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
    emit dataSetShowNumberChanged(d->dataSets[d->selectedDataSet], b);
}

void DataSetConfigWidget::ui_datasetShowPercentChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowPercentChanged(d->dataSets[d->selectedDataSet], b);
}

void DataSetConfigWidget::ui_datasetShowSymbolChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowSymbolChanged(d->dataSets[d->selectedDataSet], b);
}
