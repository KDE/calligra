/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "PieConfigWidget.h"
#include "ui_PieConfigWidget.h"

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

PieConfigWidget::PieConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
{
    init();
}

PieConfigWidget::PieConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
{
    init();
}

PieConfigWidget::~PieConfigWidget() = default;

void PieConfigWidget::init()
{
    setObjectName("PieConfigWidget");
    m_ui.setupUi(this);

    connect(m_ui.dataPoints, &QComboBox::currentIndexChanged, this, &PieConfigWidget::dataPointSelectionChanged);
    connect(m_ui.dataPointBrush, &KColorButton::changed, this, QOverload<const QColor &>::of(&PieConfigWidget::brushChanged));
    connect(m_ui.dataPointPen, &KColorButton::changed, this, QOverload<const QColor &>::of(&PieConfigWidget::penChanged));
    connect(m_ui.explodeFactor, &QSpinBox::valueChanged, this, QOverload<int>::of(&PieConfigWidget::explodeFactorChanged));
    connect(m_ui.dataPointShowCategory, &QCheckBox::toggled, this, QOverload<bool>::of(&PieConfigWidget::showCategoryChanged));
    connect(m_ui.dataPointShowNumber, &QCheckBox::toggled, this, QOverload<bool>::of(&PieConfigWidget::showNumberChanged));
    connect(m_ui.dataPointShowPercent, &QCheckBox::toggled, this, QOverload<bool>::of(&PieConfigWidget::showPercentChanged));
}

void PieConfigWidget::open(ChartShape *shape)
{
    ConfigSubWidgetBase::open(shape);
}

void PieConfigWidget::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(subtype);

    if (!chart || !chartTypes.contains(type)) {
        return;
    }
    m_dataSet = chart->plotArea()->dataSets().value(0);
    if (!m_dataSet) {
        return;
    }
    int index = m_ui.dataPoints->currentIndex();
    blockSignals(true);
    m_ui.dataPoints->clear();
    if (m_dataSet->size() == 0) {
        return;
    }
    for (int i = 0; i < m_dataSet->size(); ++i) {
        QString title = m_dataSet->categoryData(i).toString();
        if (title.isEmpty()) {
            title = i18n("Data Point %1", i + 1);
        }
        m_ui.dataPoints->addItem(title);
    }
    blockSignals(false);

    dataPointSelectionChanged(index < 0 ? 0 : index);
}

void PieConfigWidget::dataPointSelectionChanged(int index)
{
    // Check for valid index
    qInfo() << Q_FUNC_INFO << index;
    if (index < 0) {
        return;
    }
    blockSignals(true);

    m_ui.explodeFactor->setValue((int)(m_dataSet->pieAttributes(index).explodeFactor() * 100));

    m_ui.dataPointBrush->setColor(m_dataSet->brush(index).color());

    m_ui.dataPointPen->setColor(m_dataSet->pen(index).color());

    m_ui.dataPointShowCategory->setChecked(m_dataSet->valueLabelType(index).category);

    m_ui.dataPointShowNumber->setChecked(m_dataSet->valueLabelType(index).number);

    m_ui.dataPointShowPercent->setChecked(m_dataSet->valueLabelType(index).percentage);

    m_ui.dataPoints->setCurrentIndex(index);

    blockSignals(false);
}

void PieConfigWidget::brushChanged(const QColor &color)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT brushChanged(m_dataSet, color, index);
}

void PieConfigWidget::penChanged(const QColor &color)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT penChanged(m_dataSet, color, index);
}

void PieConfigWidget::explodeFactorChanged(int percent)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT explodeFactorChanged(m_dataSet, index, percent);
}

void PieConfigWidget::showCategoryChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT showCategoryChanged(m_dataSet, b, index);
}

void PieConfigWidget::showNumberChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT showNumberChanged(m_dataSet, b, index);
}

void PieConfigWidget::showPercentChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    Q_EMIT showPercentChanged(m_dataSet, b, index);
}
