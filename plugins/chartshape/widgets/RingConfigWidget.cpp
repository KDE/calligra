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
#include "RingConfigWidget.h"
#include "ui_RingConfigWidget.h"

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

RingConfigWidget::~RingConfigWidget()
{
}

void RingConfigWidget::init()
{
    setObjectName("RingConfigWidget");
    m_ui.setupUi(this);

    connect(m_ui.categories, SIGNAL(currentIndexChanged(int)), this, SLOT(categorySelectionChanged(int)));
    connect(m_ui.dataSets, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSetSelectionChanged(int)));
    connect(m_ui.dataSetBrush, SIGNAL(changed(QColor)), this, SLOT(brushChanged(QColor)));
    connect(m_ui.dataSetPen, SIGNAL(changed(QColor)), this, SLOT(penChanged(QColor)));
    connect(m_ui.explodeFactor, SIGNAL(valueChanged(int)), this, SLOT(explodeFactorChanged(int)));
    connect(m_ui.dataSetShowCategory, SIGNAL(toggled(bool)), this, SLOT(showCategoryChanged(bool)));
    connect(m_ui.dataSetShowNumber, SIGNAL(toggled(bool)), this, SLOT(showNumberChanged(bool)));
    connect(m_ui.dataSetShowPercent, SIGNAL(toggled(bool)), this, SLOT(showPercentChanged(bool)));
}
void RingConfigWidget::open(ChartShape* shape)
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
    m_ui.explodeFactor->setValue((int)(ds->pieAttributes().explodeFactor()*100));

    m_ui.dataSets->setCurrentIndex(index);
    blockSignals(false);
}

void RingConfigWidget::brushChanged(const QColor& color)
{
    int section = m_ui.categories->currentIndex();
    if (section >= 0) {
        emit brushChanged(0, color, section);
    }
}

void RingConfigWidget::penChanged(const QColor& color)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        emit penChanged(ds, color, section);
    }
}

void RingConfigWidget::explodeFactorChanged(int percent)
{
    int index = m_ui.dataSets->currentIndex();
    DataSet *ds = m_dataSets.value(index);
    if (ds) {
        emit explodeFactorChanged(ds, -1, percent);
    }
}

void RingConfigWidget::showCategoryChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        emit showCategoryChanged(ds, b, section);
    }
}

void RingConfigWidget::showNumberChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        emit showNumberChanged(ds, b, section);
    }
}

void RingConfigWidget::showPercentChanged(bool b)
{
    DataSet *ds = m_dataSets.value(0);
    int section = m_ui.categories->currentIndex();
    if (ds && section >= 0) {
        emit showPercentChanged(ds, b, section);
    }
}
