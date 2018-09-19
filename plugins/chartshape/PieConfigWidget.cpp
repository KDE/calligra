/* This file is part of the KDE project

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
#include "PieConfigWidget.h"
#include "ui_PieConfigWidget.h"

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
#include <KChartPieAttributes>
#include <KChartAbstractCartesianDiagram>
#include <KChartLegend>
#include <KChartDataValueAttributes>
#include <KChartMeasure>

// KoChart
#include "ChartProxyModel.h"
#include "PlotArea.h"
#include "DataSet.h"
#include "ChartDebug.h"

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

PieConfigWidget::~PieConfigWidget()
{
}

void PieConfigWidget::init()
{
    setObjectName("PieConfigWidget");
    m_ui.setupUi(this);

    connect(m_ui.dataPoints, SIGNAL(currentIndexChanged(int)), this, SLOT(dataPointSelectionChanged(int)));
    connect(m_ui.dataPointBrush, SIGNAL(changed(QColor)), this, SLOT(brushChanged(QColor)));
    connect(m_ui.dataPointPen, SIGNAL(changed(QColor)), this, SLOT(penChanged(QColor)));
    connect(m_ui.explodeFactor, SIGNAL(valueChanged(int)), this, SLOT(explodeFactorChanged(int)));
    connect(m_ui.dataPointShowCategory, SIGNAL(toggled(bool)), this, SLOT(showCategoryChanged(bool)));
    connect(m_ui.dataPointShowNumber, SIGNAL(toggled(bool)), this, SLOT(showNumberChanged(bool)));
    connect(m_ui.dataPointShowPercent, SIGNAL(toggled(bool)), this, SLOT(showPercentChanged(bool)));
}
void PieConfigWidget::open(ChartShape* shape)
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
    qInfo()<<Q_FUNC_INFO<<index;
    if (index < 0) {
        return;
    }
    blockSignals(true);

    m_ui.explodeFactor->setValue((int)(m_dataSet->pieAttributes(index).explodeFactor()*100));

    m_ui.dataPointBrush->setColor(m_dataSet->brush(index).color());

    m_ui.dataPointPen->setColor(m_dataSet->pen(index).color());

    m_ui.dataPointShowCategory->setChecked(m_dataSet->valueLabelType(index).category);

    m_ui.dataPointShowNumber->setChecked(m_dataSet->valueLabelType(index).number);

    m_ui.dataPointShowPercent->setChecked(m_dataSet->valueLabelType(index).percentage);

    m_ui.dataPoints->setCurrentIndex(index);

    blockSignals(false);

}

void PieConfigWidget::brushChanged(const QColor& color)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit brushChanged(m_dataSet, color, index);
}

void PieConfigWidget::penChanged(const QColor& color)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit penChanged(m_dataSet, color, index);
}

void PieConfigWidget::explodeFactorChanged(int percent)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit explodeFactorChanged(m_dataSet, index, percent);
}

void PieConfigWidget::showCategoryChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit showCategoryChanged(m_dataSet, b, index);    
}

void PieConfigWidget::showNumberChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit showNumberChanged(m_dataSet, b, index);
}

void PieConfigWidget::showPercentChanged(bool b)
{
    int index = m_ui.dataPoints->currentIndex();
    if (index < 0) {
        return;
    }
    emit showPercentChanged(m_dataSet, b, index);
}
