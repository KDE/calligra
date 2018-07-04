/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin    <inge@lysator.liu.se>

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
#include "AxesConfigWidget.h"
#include "ui_AxesConfigWidget.h"

// Qt
#include <QAction>

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
#include <KChartDataValueAttributes>
#include <KChartTextAttributes>
#include <KChartMarkerAttributes>
#include <KChartMeasure>

// KoChart
#include "ChartProxyModel.h"
#include "PlotArea.h"
#include "Axis.h"
#include "ui_AxesConfigWidget.h"
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


class AxesConfigWidget::Private
{
public:
    Private(QWidget *parent);
    ~Private();

    Ui::AxesConfigWidget  ui;

    QList<Axis*>    dataSetAxes;
    QList<Axis*>    axes;
    QList<DataSet*> dataSets;

    // Dialogs
    NewAxisDialog     newAxisDialog;
    AxisScalingDialog axisScalingDialog;
    FontEditorDialog axisFontEditorDialog;

};


AxesConfigWidget::Private::Private(QWidget *parent)
    : newAxisDialog(parent)
{
}

AxesConfigWidget::Private::~Private()
{
}

// ================================================================
//                     class AxesConfigWidget

// TODO:
// 1) Allow user to change axis' "visible" property

AxesConfigWidget::AxesConfigWidget()
    : d(new Private(this))
{
    setObjectName("AxesConfigWidget");
    d->ui.setupUi(this);

    d->ui.axisPosition->insertItem(0, i18n("Start"), "start");
    d->ui.axisPosition->insertItem(1, i18n("End"), "end");

    d->ui.axislabelPosition->insertItem(0, i18n("Near-axis"), "near-axis");
    d->ui.axislabelPosition->insertItem(1, i18n("Other-side"), "near-axis-other-side");
    d->ui.axislabelPosition->insertItem(2, i18n("End"), "outside-end");
    d->ui.axislabelPosition->insertItem(3, i18n("Start"), "outside-start");


    connect(d->ui.axisShowTitle, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowTitleChanged(bool)));
    connect(d->ui.axisShow, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowChanged(bool)));
    connect(d->ui.axisPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(ui_axisPositionChanged(int)));
    connect(d->ui.axislabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(ui_axisLabelsPositionChanged(int)));
    connect(d->ui.axisShowMajorGridLines, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowMajorGridLinesChanged(bool)));
    connect(d->ui.axisShowMinorGridLines, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowMinorGridLinesChanged(bool)));
    connect(d->ui.axes, SIGNAL(currentIndexChanged(int)),this, SLOT(ui_axisSelectionChanged(int)));

    connect(d->ui.gapBetweenBars, SIGNAL(editingFinished()), this, SLOT(slotGapBetweenBars()));
    connect(d->ui.gapBetweenSets, SIGNAL(editingFinished()), this, SLOT(slotGapBetweenSets()));

    setupDialogs();
    createActions();
}

AxesConfigWidget::~AxesConfigWidget()
{
    delete d;
}

Axis *AxesConfigWidget::axis(int index) const
{
    Axis *a = 0;
    switch (index) {
        case 0: a = chart->plotArea()->xAxis(); break;
        case 1: a = chart->plotArea()->yAxis(); break;
        case 2: a = chart->plotArea()->secondaryXAxis(); break;
        case 3: a = chart->plotArea()->secondaryYAxis(); break;
        default:
            Q_ASSERT(false);
            break;
    }
    return a;
}

void AxesConfigWidget::deleteSubDialogs()
{
}

void AxesConfigWidget::open(KoShape* shape)
{
    debugChartUiAxes<<shape;
    d->axes.clear();
    ConfigWidgetBase::open(shape);
}

QAction * AxesConfigWidget::createAction()
{
    return 0;
}

void AxesConfigWidget::updateData()
{
    debugChartUiAxes<<chart<<d->ui.axes->currentIndex();
    if (!chart) {
        return;
    }
    blockSignals(true);
    d->ui.axisShow->setChecked(false);
    d->ui.axisShowMajorGridLines->setChecked(false);
    d->ui.axisShowMinorGridLines->setChecked(false);
    d->ui.axisShowTitle->setChecked(false);

    if (d->ui.axes->currentIndex() < 0) {
        d->ui.axes->setCurrentIndex(0);
    }
    blockSignals(false);
    ui_axisSelectionChanged(d->ui.axes->currentIndex());
}


void AxesConfigWidget::setupDialogs()
{
    // Axis scaling
    connect(d->ui.axisScalingButton, SIGNAL(clicked()),
             this, SLOT(ui_axisScalingButtonClicked()));
    connect(d->axisScalingDialog.logarithmicScaling, SIGNAL(toggled(bool)),
             this, SLOT(ui_axisUseLogarithmicScalingChanged(bool)));
    connect(d->axisScalingDialog.stepWidth, SIGNAL(valueChanged(double)),
             this, SLOT(ui_axisStepWidthChanged(double)));
    connect (d->axisScalingDialog.automaticStepWidth, SIGNAL(toggled(bool)),
              this, SLOT(ui_axisUseAutomaticStepWidthChanged(bool)));
    connect(d->axisScalingDialog.subStepWidth, SIGNAL(valueChanged(double)),
             this, SLOT(ui_axisSubStepWidthChanged(double)));
    connect (d->axisScalingDialog.automaticSubStepWidth, SIGNAL(toggled(bool)),
              this, SLOT(ui_axisUseAutomaticSubStepWidthChanged(bool)));

    // Edit Fonts
    connect(d->ui.axisEditFontButton, SIGNAL(clicked()),
             this, SLOT(ui_axisEditFontButtonClicked()));
    connect(&d->axisFontEditorDialog, SIGNAL(accepted()),
             this, SLOT(ui_axisLabelsFontChanged()));

}

void AxesConfigWidget::createActions()
{
}

void AxesConfigWidget::ui_axisSelectionChanged(int index)
{
    Q_ASSERT(chart);
    // Check for valid index
    if (index < 0 || index > 3) {
        warnChartUiAxes<<"Invalid axis index"<<index;
        return;
    }
    Axis *axis = 0;
    switch (index) {
        case 0:
            axis = chart->plotArea()->xAxis();
            break;
        case 1:
            axis = chart->plotArea()->yAxis();
            break;
        case 2:
            axis = chart->plotArea()->secondaryXAxis();
            if (!axis) {
                debugChartUiAxes<<"create secondary x axis";
                emit axisAdded(XAxisDimension, i18n("Axistitle"));
                return;
            }
            break;
        case 3:
            axis = chart->plotArea()->secondaryYAxis();
            if (!axis) {
                debugChartUiAxes<<"create secondary y axis";
                emit axisAdded(YAxisDimension, i18n("Axistitle"));
                return;
            }
            break;
        default:
            Q_ASSERT(false);
            break;
    }
    blockSignals(true);
    d->ui.axisShow->setChecked(axis->isVisible());
    d->ui.axisShowTitle->setChecked(axis->title()->isVisible());
    for (int i = 0; i < d->ui.axisPosition->count(); ++i) {
        if (d->ui.axisPosition->itemData(i).toString() == axis->odfAxisPosition()) {
            d->ui.axisPosition->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < d->ui.axislabelPosition->count(); ++i) {
        if (d->ui.axislabelPosition->itemData(i).toString() == axis->odfAxisLabelsPosition()) {
            d->ui.axislabelPosition->setCurrentIndex(i);
            break;
        }
    }
    d->ui.axisShowMajorGridLines->setChecked(axis->showMajorGrid());
    d->ui.axisShowMinorGridLines->setChecked(axis->showMinorGrid());

    blockSignals(&d->axisScalingDialog, true);
    if (axis->dimension() == YAxisDimension) {
        d->axisScalingDialog.logarithmicScaling->setEnabled(true);
    } else {
        d->axisScalingDialog.logarithmicScaling->setEnabled(false);
    }

    d->axisScalingDialog.stepWidth->setValue(axis->majorInterval());

    d->axisScalingDialog.subStepWidth->setValue(axis->minorInterval());

    d->axisScalingDialog.automaticStepWidth->setChecked(axis->useAutomaticMajorInterval());
    d->axisScalingDialog.stepWidth->setEnabled(!axis->useAutomaticMajorInterval());

    d->axisScalingDialog.automaticSubStepWidth->setChecked(axis->useAutomaticMinorInterval());
    d->axisScalingDialog.subStepWidth->setEnabled(!axis->useAutomaticMinorInterval());
    blockSignals(&d->axisScalingDialog, false);

    d->ui.gapBetweenBars->setValue(axis->gapBetweenBars());
    d->ui.gapBetweenSets->setValue(axis->gapBetweenSets());
    d->ui.barProperties->setVisible(axis->dimension() == YAxisDimension);
    blockSignals(false);
}


void AxesConfigWidget::ui_axisShowTitleChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        emit axisShowTitleChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisShowChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisShowChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisPositionChanged(int index)
{
    Axis *a = axis(d->ui.axisPosition->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<index;
        emit axisPositionChanged(a, d->ui.axisPosition->currentData().toString());
    }
}

void AxesConfigWidget::ui_axisLabelsPositionChanged(int index)
{
    Axis *a = axis(d->ui.axislabelPosition->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<index;
        emit axisLabelsPositionChanged(a, d->ui.axislabelPosition->currentData().toString());
    }
}


void AxesConfigWidget::ui_axisShowMajorGridLinesChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisShowMajorGridLinesChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisShowMinorGridLinesChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisShowMinorGridLinesChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisLabelsFontChanged()
{
    QFont font = d->axisFontEditorDialog.fontChooser->font();
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<font;
        emit axisLabelsFontChanged(a, font);
    }
}

void AxesConfigWidget::ui_axisUseLogarithmicScalingChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisUseLogarithmicScalingChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisStepWidthChanged(double width)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<width;
        emit axisStepWidthChanged(a, width);
    }
}

void AxesConfigWidget::ui_axisSubStepWidthChanged(double width)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<width;
        emit axisSubStepWidthChanged(a, width);
    }
}

void AxesConfigWidget::ui_axisUseAutomaticStepWidthChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisUseAutomaticStepWidthChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisUseAutomaticSubStepWidthChanged(bool b)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<b;
        emit axisUseAutomaticSubStepWidthChanged(a, b);
    }
}

void AxesConfigWidget::ui_axisScalingButtonClicked()
{
    d->axisScalingDialog.show();
}

void AxesConfigWidget::ui_axisEditFontButtonClicked()
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        QFont font = a->font();
        d->axisFontEditorDialog.fontChooser->setFont(font);
        d->axisFontEditorDialog.show();
    }
}

void AxesConfigWidget::slotGapBetweenBars()
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a && a->dimension() == YAxisDimension) {
        debugChartUiAxes<<a<<d->ui.gapBetweenBars->value();
        emit gapBetweenBarsChanged(a, d->ui.gapBetweenBars->value());
    }
}

void AxesConfigWidget::slotGapBetweenSets()
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a && a->dimension() == YAxisDimension) {
        debugChartUiAxes<<a<<d->ui.gapBetweenSets->value();
        emit gapBetweenSetsChanged(a, d->ui.gapBetweenSets->value());
    }
}
