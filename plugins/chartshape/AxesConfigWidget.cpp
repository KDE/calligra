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

    // The owner of this struct.
    ChartShape            *shape;

    Ui::AxesConfigWidget  ui;

    QList<Axis*>    dataSetAxes;
    QList<Axis*>    axes;
    QList<DataSet*> dataSets;

    // Dialogs
    NewAxisDialog     newAxisDialog;
    AxisScalingDialog axisScalingDialog;
    FontEditorDialog axisFontEditorDialog;

    Axis *axis(int index) const;
};


AxesConfigWidget::Private::Private(QWidget *parent)
    : newAxisDialog(parent)
{
    shape = 0;
}

AxesConfigWidget::Private::~Private()
{
}

Axis *AxesConfigWidget::Private::axis(int index) const
{
    Axis *axis = 0;
    switch (index) {
        case 0: axis = shape->plotArea()->xAxis(); break;
        case 1: axis = shape->plotArea()->yAxis(); break;
        case 2: axis = shape->plotArea()->secondaryXAxis(); break;
        case 3: axis = shape->plotArea()->secondaryYAxis(); break;
        default:
            Q_ASSERT(false);
            break;
    }
    return axis;
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


    connect(d->ui.axisShowTitle, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowTitleChanged(bool)));
    connect(d->ui.axisShow, SIGNAL(toggled(bool)), this, SLOT(ui_axisShowChanged(bool)));
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

void AxesConfigWidget::deleteSubDialogs()
{
}

void AxesConfigWidget::open(KoShape* shape)
{
    debugChartUiAxes<<shape;
    d->shape = 0;
    d->axes.clear();

    d->shape = dynamic_cast<ChartShape*>(shape);
    if (!d->shape) {
        d->shape = dynamic_cast<ChartShape*>(shape->parent());
        if (!d->shape) {
            return;
        }
    }
    updateData();
}

QAction * AxesConfigWidget::createAction()
{
    return 0;
}


static QString nonEmptyAxisTitle(Axis *axis, int index)
{
    QString title = axis->titleText();
    if (title.isEmpty())
        // TODO (post-2.3): Use "X Axis" or "Y Axis" as default labels instead
        title = i18n("Axis %1", index + 1);
    return title;
}

void AxesConfigWidget::updateData()
{
    debugChartUiAxes<<d->shape<<d->ui.axes->currentIndex();
    if (!d->shape) {
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
    Q_ASSERT(d->shape);
    // Check for valid index
    if (index < 0 || index > 3) {
        warnChartUiAxes<<"Invalid axis index"<<index;
        return;
    }
    Axis *axis = 0;
    switch (index) {
        case 0:
            axis = d->shape->plotArea()->xAxis();
            break;
        case 1:
            axis = d->shape->plotArea()->yAxis();
            break;
        case 2:
            axis = d->shape->plotArea()->secondaryXAxis();
            if (!axis) {
                debugChartUiAxes<<"create secondary x axis";
                emit axisAdded(XAxisDimension, "TITLE");
                return;
            }
            break;
        case 3:
            axis = d->shape->plotArea()->secondaryYAxis();
            if (!axis) {
                debugChartUiAxes<<"create secondary y axis";
                emit axisAdded(YAxisDimension, "TITLE");
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
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        emit axisShowTitleChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisShowChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisShowChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisShowMajorGridLinesChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisShowMajorGridLinesChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisShowMinorGridLinesChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisShowMinorGridLinesChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisLabelsFontChanged()
{
    QFont font = d->axisFontEditorDialog.fontChooser->font();
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<font;
        emit axisLabelsFontChanged(axis, font);
    }
}

void AxesConfigWidget::ui_axisUseLogarithmicScalingChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisUseLogarithmicScalingChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisStepWidthChanged(double width)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<width;
        emit axisStepWidthChanged(axis, width);
    }
}

void AxesConfigWidget::ui_axisSubStepWidthChanged(double width)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<width;
        emit axisSubStepWidthChanged(axis, width);
    }
}

void AxesConfigWidget::ui_axisUseAutomaticStepWidthChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisUseAutomaticStepWidthChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisUseAutomaticSubStepWidthChanged(bool b)
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        debugChartUiAxes<<axis<<b;
        emit axisUseAutomaticSubStepWidthChanged(axis, b);
    }
}

void AxesConfigWidget::ui_axisScalingButtonClicked()
{
    d->axisScalingDialog.show();
}

void AxesConfigWidget::ui_axisEditFontButtonClicked()
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis) {
        QFont font = axis->font();
        d->axisFontEditorDialog.fontChooser->setFont(font);
        d->axisFontEditorDialog.show();
    }
}

void AxesConfigWidget::slotGapBetweenBars()
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis && axis->dimension() == YAxisDimension) {
        debugChartUiAxes<<axis<<d->ui.gapBetweenBars->value();
        emit gapBetweenBarsChanged(axis, d->ui.gapBetweenBars->value());
    }
}

void AxesConfigWidget::slotGapBetweenSets()
{
    Axis *axis = d->axis(d->ui.axes->currentIndex());
    if (axis && axis->dimension() == YAxisDimension) {
        debugChartUiAxes<<axis<<d->ui.gapBetweenSets->value();
        emit gapBetweenSetsChanged(axis, d->ui.gapBetweenSets->value());
    }
}
