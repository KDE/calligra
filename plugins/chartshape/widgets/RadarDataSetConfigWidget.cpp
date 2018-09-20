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
#include "RadarDataSetConfigWidget.h"
#include "ui_RadarDataSetConfigWidget.h"

// Qt
#include <QAction>
#include <QLatin1String>
#include <QMenu>

// KF5
#include <klocalizedstring.h>

// Calligra

// KChart

// KoChart
#include "ChartProxyModel.h"
#include "DataSet.h"
#include "PlotArea.h"
#include "ChartDebug.h"

using namespace KoChart;


class RadarDataSetConfigWidget::Private
{
public:
    Private(RadarDataSetConfigWidget *parent);
    ~Private();

    RadarDataSetConfigWidget *q;

    Ui::RadarDataSetConfigWidget ui;

    QList<DataSet*> dataSets;
    int selectedDataSet;

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
};


RadarDataSetConfigWidget::Private::Private(RadarDataSetConfigWidget *parent)
    : q(parent)
{
    ui.setupUi(parent);

    selectedDataSet = 0;

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

    connect(ui.datasetBrush, SIGNAL(changed(QColor)),parent, SLOT(datasetBrushSelected(QColor)));
    connect(ui.datasetPen, SIGNAL(changed(QColor)), parent, SLOT(datasetPenSelected(QColor)));
    connect(ui.datasetShowCategory, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowCategoryChanged(bool)));
    connect(ui.dataSetShowNumber, SIGNAL(toggled(bool)), parent, SLOT(ui_dataSetShowNumberChanged(bool)));
    connect(ui.datasetShowPercent, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowPercentChanged(bool)));
    // TODO KChart does not support
    // connect(ui.datasetShowSymbol, SIGNAL(toggled(bool)), parent, SLOT(ui_datasetShowSymbolChanged(bool)));


    connect(ui.dataSets, SIGNAL(currentIndexChanged(int)), parent, SLOT(ui_dataSetSelectionChanged(int)));

}

RadarDataSetConfigWidget::Private::~Private()
{
}


// ================================================================
//                     class RadarDataSetConfigWidget


RadarDataSetConfigWidget::RadarDataSetConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
    , d(new Private(this))
{
    setObjectName("RadarDataSetConfigWidget");
}

RadarDataSetConfigWidget::RadarDataSetConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
    , d(new Private(this))
{
    setObjectName("RadarDataSetConfigWidget");
}

RadarDataSetConfigWidget::~RadarDataSetConfigWidget()
{
    delete d;
}

void RadarDataSetConfigWidget::deleteSubDialogs(ChartType type)
{
    Q_UNUSED(type)
}

void RadarDataSetConfigWidget::open(ChartShape* shape)
{
    debugChartUiDataSet<<shape;

    d->dataSets.clear();

    ConfigSubWidgetBase::open(shape);
}

void RadarDataSetConfigWidget::updateMarkers()
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

void RadarDataSetConfigWidget::datasetMarkerSelected(QAction *action)
{
    if (d->selectedDataSet < 0)
        return;

    const int numDefaultMarkerTypes = 15;
    OdfMarkerStyle style = MarkerSquare;
    QString type = QString("");
    OdfSymbolType symbolType = NamedSymbol;
    if (action == d->dataSetNoMarkerAction) {
        type = "None";
        symbolType = NoSymbol;
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
    if (!dataSet) {
        return;
    }
    if (type.isEmpty()) {
        d->ui.datasetMarkerMenu->setIcon(dataSet->markerIcon(style));
        d->ui.datasetMarkerMenu->setText("");
    } else {
        d->ui.datasetMarkerMenu->setText(type);
        d->ui.datasetMarkerMenu->setIcon(QIcon());
    }
    emit dataSetMarkerChanged(dataSet, symbolType, style);
}

void RadarDataSetConfigWidget::datasetBrushSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetBrushChanged(d->dataSets[d->selectedDataSet], color, -1);
    updateMarkers();
}

void RadarDataSetConfigWidget::datasetPenSelected(const QColor& color)
{
    if (d->selectedDataSet < 0)
        return;

    emit datasetPenChanged(d->dataSets[d->selectedDataSet], color, -1);
    updateMarkers();
}

void RadarDataSetConfigWidget::updateData(ChartType chartType, ChartSubtype subtype)
{
    Q_UNUSED(subtype)
    if (!chart || !chartTypes.contains(chartType)) {
        return;
    }

    blockSignals(true);
    d->ui.dataSets->clear();

    // always update, in case titles have changed
    QList<DataSet*> newDataSets = chart->plotArea()->dataSets();
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

void RadarDataSetConfigWidget::ui_dataSetSelectionChanged(int index)
{
    // Check for valid index
    debugChartUiDataSet<<index<<d->dataSets;
    if (index < 0 || index >= d->dataSets.size()) {
        return;
    }
    blockSignals(true);

    DataSet *dataSet = d->dataSets[index];
    d->ui.datasetBrush->setColor(dataSet->brush().color());

    d->ui.datasetPen->setColor(dataSet->pen().color());

    d->ui.datasetShowCategory->setChecked(dataSet->valueLabelType().category);

    d->ui.dataSetShowNumber->setChecked(dataSet->valueLabelType().number);

    d->ui.datasetShowPercent->setChecked(dataSet->valueLabelType().percentage);

    // TODO: KChart does not support
    // d->ui.datasetShowSymbol->setChecked(dataSet->valueLabelType().symbol);

    d->selectedDataSet = index;

    blockSignals(false);

    updateMarkers();
}

void RadarDataSetConfigWidget::ui_datasetShowCategoryChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowCategoryChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void RadarDataSetConfigWidget::ui_dataSetShowNumberChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit dataSetShowNumberChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void RadarDataSetConfigWidget::ui_datasetShowPercentChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowPercentChanged(d->dataSets[d->selectedDataSet], b, -1);
}

void RadarDataSetConfigWidget::ui_datasetShowSymbolChanged(bool b)
{
    if (d->selectedDataSet < 0 || d->selectedDataSet >= d->dataSets.count()) {
        return;
    }
    debugChartUiDataSet<<b;
    emit datasetShowSymbolChanged(d->dataSets[d->selectedDataSet], b, -1);
}
