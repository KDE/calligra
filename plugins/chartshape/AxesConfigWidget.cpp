/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "AxesConfigWidget.h"
#include "ui_AxesConfigWidget.h"

// Qt
#include <QAction>

// KF5
#include <KLocalizedString>
#include <KMessageBox>
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
    Private(AxesConfigWidget *parent);
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


AxesConfigWidget::Private::Private(AxesConfigWidget *parent)
    : newAxisDialog(parent)
{
    ui.setupUi(parent);
    ui.axisPosition->insertItem(0, i18n("Start"), "start");
    ui.axisPosition->insertItem(1, i18n("End"), "end");

    ui.axislabelPosition->insertItem(0, i18n("Near-axis"), "near-axis");
    ui.axislabelPosition->insertItem(1, i18n("Other-side"), "near-axis-other-side");
    ui.axislabelPosition->insertItem(2, i18n("End"), "outside-end");
    ui.axislabelPosition->insertItem(3, i18n("Start"), "outside-start");


    connect(ui.axisShowTitle, &QAbstractButton::toggled, parent, &AxesConfigWidget::ui_axisShowTitleChanged);
    connect(ui.axisShow, &QAbstractButton::toggled, parent, &AxesConfigWidget::ui_axisShowChanged);
    connect(ui.axisPosition, &QComboBox::currentIndexChanged, parent, &AxesConfigWidget::ui_axisPositionChanged);
    connect(ui.axislabelPosition, &QComboBox::currentIndexChanged, parent, &AxesConfigWidget::ui_axisLabelsPositionChanged);
    connect(ui.axisShowLabels, &QAbstractButton::toggled, parent, &AxesConfigWidget::ui_axisShowLabelsChanged);
    connect(ui.axisShowMajorGridLines, &QAbstractButton::toggled, parent, &AxesConfigWidget::ui_axisShowMajorGridLinesChanged);
    connect(ui.axisShowMinorGridLines, &QAbstractButton::toggled, parent, &AxesConfigWidget::ui_axisShowMinorGridLinesChanged);
    connect(ui.axes, &QComboBox::currentIndexChanged, parent, &AxesConfigWidget::ui_axisSelectionChanged);

    connect(ui.gapBetweenBars, &QAbstractSpinBox::editingFinished, parent, &AxesConfigWidget::slotGapBetweenBars);
    connect(ui.gapBetweenSets, &QAbstractSpinBox::editingFinished, parent, &AxesConfigWidget::slotGapBetweenSets);
}

AxesConfigWidget::Private::~Private()
{
}

// ================================================================
//                     class AxesConfigWidget

// TODO:
// 1) Allow user to change axis' "visible" property

AxesConfigWidget::AxesConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
    , d(new Private(this))
{
    setObjectName("AxesConfigWidget");

    setupDialogs();
    createActions();
}

AxesConfigWidget::AxesConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
    , d(new Private(this))
{
    setObjectName("AxesConfigWidget");

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

void AxesConfigWidget::deleteSubDialogs(ChartType type)
{
    Q_UNUSED(type)
}

void AxesConfigWidget::open(ChartShape* shape)
{
    debugChartUiAxes<<shape;
    d->axes.clear();
    ConfigSubWidgetBase::open(shape);
}

void AxesConfigWidget::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(subtype);

    debugChartUiAxes<<chart<<d->ui.axes->currentIndex();
    if (!chart || !chartTypes.contains(type)) {
        return;
    }
    show();
    blockSignals(true);
    d->ui.axisShow->setChecked(false);
    d->ui.axisShowMajorGridLines->setChecked(false);
    d->ui.axisShowMinorGridLines->setChecked(false);
    d->ui.axisShowTitle->setChecked(false);

    switch (d->ui.axes->currentIndex()) {
        case 0:
        case 1:
            // always ok
            break;
        case 2:
            // ensure we do not point to a removed axis
            if (!chart->plotArea()->secondaryXAxis()) {
                d->ui.axes->setCurrentIndex(0);
            }
            break;
        case 3:
            // ensure we do not point to a removed axis
            if (!chart->plotArea()->secondaryYAxis()) {
                d->ui.axes->setCurrentIndex(0);
            }
            break;
        default:
            d->ui.axes->setCurrentIndex(0);
            break;
    }
    blockSignals(false);
    ui_axisSelectionChanged(d->ui.axes->currentIndex());
}


void AxesConfigWidget::setupDialogs()
{
    // Axis scaling
    connect(d->ui.axisScalingButton, &QAbstractButton::clicked,
             this, &AxesConfigWidget::ui_axisScalingButtonClicked);
    connect(d->axisScalingDialog.logarithmicScaling, &QAbstractButton::toggled,
             this, &AxesConfigWidget::ui_axisUseLogarithmicScalingChanged);
    connect(d->axisScalingDialog.stepWidth, &QDoubleSpinBox::valueChanged,
             this, &AxesConfigWidget::ui_axisStepWidthChanged);
    connect (d->axisScalingDialog.automaticStepWidth, &QAbstractButton::toggled,
              this, &AxesConfigWidget::ui_axisUseAutomaticStepWidthChanged);
    connect(d->axisScalingDialog.subStepWidth, &QDoubleSpinBox::valueChanged,
             this, &AxesConfigWidget::ui_axisSubStepWidthChanged);
    connect (d->axisScalingDialog.automaticSubStepWidth, &QAbstractButton::toggled,
              this, &AxesConfigWidget::ui_axisUseAutomaticSubStepWidthChanged);

    // Edit Fonts
    connect(d->ui.axisEditFontButton, &QAbstractButton::clicked,
             this, &AxesConfigWidget::ui_axisEditFontButtonClicked);
    connect(&d->axisFontEditorDialog, &QDialog::accepted,
             this, &AxesConfigWidget::ui_axisLabelsFontChanged);

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
    debugChartUiAxes<<axis<<axis->odfAxisPosition()<<axis->odfAxisLabelsPosition();
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
    d->ui.axisShowLabels->setChecked(axis->showLabels());
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
    d->ui.barProperties->setVisible(chart->chartType() == BarChartType && axis->dimension() == YAxisDimension);
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
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<index;
        emit axisPositionChanged(a, d->ui.axisPosition->currentData().toString());
    }
}

void AxesConfigWidget::ui_axisLabelsPositionChanged(int index)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<index;
        emit axisLabelsPositionChanged(a, d->ui.axislabelPosition->currentData().toString());
    }
}

void AxesConfigWidget::ui_axisShowLabelsChanged(bool value)
{
    Axis *a = axis(d->ui.axes->currentIndex());
    if (a) {
        debugChartUiAxes<<a<<value;
        emit axisShowLabelsChanged(a, value);
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
