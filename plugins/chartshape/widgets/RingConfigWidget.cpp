/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "RingConfigWidget.h"
#include "ui_RingConfigWidget.h"

// Qt
#include <QAction>
#include <QLatin1String>
#include <QMenu>

// KF5
#include <KLocalizedString>
#include <KMessageBox>
#include <kfontchooser.h>

// Calligra
#include <KoIcon.h>
#include <interfaces/KoChartModel.h>

// KChart
#include <KChartAbstractCartesianDiagram>
#include <KChartCartesianAxis>
#include <KChartChart>
#include <KChartDataValueAttributes>
#include <KChartGridAttributes>
#include <KChartLegend>
#include <KChartMeasure>
#include <KChartPieAttributes>
#include <KChartPosition>

// KoChart
#include "ChartDebug.h"
#include "ChartProxyModel.h"
#include "DataSet.h"
#include "PlotArea.h"

using namespace KoChart;

RingConfigWidget::RingConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
{
    init();
}

RingConfigWidget::RingConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
{
    init();
}

RingConfigWidget::~RingConfigWidget() = default;

void RingConfigWidget::init()
{
    setObjectName("RingConfigWidget");
    m_ui.setupUi(this);

    connect(m_ui.categories, &QComboBox::currentIndexChanged, this, &RingConfigWidget::categorySelectionChanged);
    connect(m_ui.dataSets, &QComboBox::currentIndexChanged, this, &RingConfigWidget::dataSetSelectionChanged);
    connect(m_ui.dataSetPen, &KColorButton::changed, this, QOverload<const QColor &>::of(&RingConfigWidget::penChanged));
    connect(m_ui.dataSetBrush, &KColorButton::changed, this, QOverload<const QColor &>::of(&RingConfigWidget::brushChanged));
    connect(m_ui.explodeFactor, &QSpinBox::valueChanged, this, QOverload<int>::of(&RingConfigWidget::explodeFactorChanged));
    connect(m_ui.dataSetShowCategory, &QCheckBox::toggled, this, QOverload<bool>::of(&RingConfigWidget::showCategoryChanged));
    connect(m_ui.dataSetShowNumber, &QCheckBox::toggled, this, QOverload<bool>::of(&RingConfigWidget::showNumberChanged));
    connect(m_ui.dataSetShowPercent, &QCheckBox::toggled, this, QOverload<bool>::of(&RingConfigWidget::showPercentChanged));
}
void RingConfigWidget::open(ChartShape *shape)
{
    ConfigSubWidgetBase::open(shape);
}

void RingConfigWidget::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(subtype);

    if (!chart || !chartTypes.contains(type)) {
        return;
    }
    bool dsequal = m_dataSets == chart->plotArea()->dataSets();
    m_dataSets = chart->plotArea()->dataSets();
    if (m_dataSets.isEmpty()) {
        return;
    }
    // categories
    int cindex = m_ui.categories->currentIndex();
    blockSignals(true);
    m_ui.categories->clear();
    DataSet *ds = m_dataSets.at(0);
    for (int i = 0; i < ds->size(); ++i) {
        QString title = ds->categoryData(i).toString();
        if (title.isEmpty()) {
            title = i18n("Row %1", i + 1);
        }
        m_ui.categories->addItem(title);
    }
    // datasets
    int dsindex = m_ui.dataSets->currentIndex();
    m_ui.dataSets->clear();
    for (int i = 0; i < m_dataSets.count(); ++i) {
        m_ui.dataSets->addItem(m_dataSets.at(i)->labelData().toString());
    }
    blockSignals(false);

    if (!dsequal || dsindex < 0) {
        dsindex = 0;
    }
    dataSetSelectionChanged(dsindex);

    if (cindex < 0 || cindex >= m_dataSets.count()) {
        cindex = 0;
    }
    categorySelectionChanged(cindex);
}

void RingConfigWidget::categorySelectionChanged(int index)
{
    // Check for valid index
    DataSet *ds = m_dataSets.value(0);
    if (!ds) {
        return;
    }
    blockSignals(true);

    m_ui.dataSetBrush->setColor(ds->brush(index).color());

    m_ui.dataSetPen->setColor(ds->pen(index).color());

    m_ui.dataSetShowCategory->setChecked(ds->valueLabelType(index).category);

    m_ui.dataSetShowNumber->setChecked(ds->valueLabelType(index).number);

    m_ui.dataSetShowPercent->setChecked(ds->valueLabelType(index).percentage);

    m_ui.categories->setCurrentIndex(index);

    blockSignals(false);
}
void RingConfigWidget::dataSetSelectionChanged(int index)
{
    DataSet *ds = m_dataSets.value(index);
    if (!ds) {
        return;
    }
    blockSignals(true);
    m_ui.explodeFactor->setValue((int)(ds->pieAttributes().explodeFactor() * 100));

    m_ui.dataSets->setCurrentIndex(index);
    blockSignals(false);
}

void RingConfigWidget::brushChanged(const QColor &color)
{
    int section = m_ui.categories->currentIndex();
    if (section >= 0) {
        Q_EMIT brushChanged(nullptr, color, section);
    }
}

void RingConfigWidget::penChanged(const QColor &color)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        Q_EMIT penChanged(ds, color, section);
    }
}

void RingConfigWidget::explodeFactorChanged(int percent)
{
    int index = m_ui.dataSets->currentIndex();
    DataSet *ds = m_dataSets.value(index);
    if (ds) {
        Q_EMIT explodeFactorChanged(ds, -1, percent);
    }
}

void RingConfigWidget::showCategoryChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        Q_EMIT showCategoryChanged(ds, b, section);
    }
}

void RingConfigWidget::showNumberChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        Q_EMIT showNumberChanged(ds, b, section);
    }
}

void RingConfigWidget::showPercentChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        Q_EMIT showPercentChanged(ds, b, section);
    }
}
