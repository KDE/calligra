/* This file is part of the KDE project
 * 
 * Copyright 2018 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "BubbleConfigWidget.h"

// Qt
#include <QAction>
#include <QMenu>
#include <QLatin1String>
#include <QColorDialog>

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


BubbleConfigWidget::BubbleConfigWidget(QWidget *parent)
    : ConfigObjectBase(parent)
{
    init();
}

BubbleConfigWidget::BubbleConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigObjectBase(types, parent)
{
    init();
}

BubbleConfigWidget::~BubbleConfigWidget()
{
}

void BubbleConfigWidget::init()
{
    setObjectName("BubbleConfigWidget");
    m_ui.setupUi(this);

    connect(m_ui.dataSets, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSetSelectionChanged(int)));
    connect(m_ui.dataSetBrush, SIGNAL(changed(QColor)), this, SLOT(brushChanged(QColor)));
    connect(m_ui.dataSetPen, SIGNAL(changed(QColor)), this, SLOT(penChanged(QColor)));
    connect(m_ui.dataSetShowCategory, SIGNAL(toggled(bool)), this, SLOT(showCategoryChanged(bool)));
    connect(m_ui.dataSetShowNumber, SIGNAL(toggled(bool)), this, SLOT(showNumberChanged(bool)));
    connect(m_ui.dataSetShowPercent, SIGNAL(toggled(bool)), this, SLOT(showPercentChanged(bool)));
    connect(m_ui.dataSetShowSymbol, SIGNAL(toggled(bool)), this, SLOT(showSymbolChanged(bool)));
}
void BubbleConfigWidget::open(ChartShape* shape)
{
    ConfigObjectBase::open(shape);
}

void BubbleConfigWidget::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(subtype);

    if (!chart || !chartTypes.contains(type)) {
        return;
    }
    m_dataSets = chart->plotArea()->dataSets();
    if (m_dataSets.isEmpty()) {
        return;
    }
    int index = m_ui.dataSets->currentIndex();
    blockSignals(true);
    m_ui.dataSets->clear();
    for (DataSet *dataSet : m_dataSets) {
        QString title = dataSet->labelData().toString();
        if (title.isEmpty()) {
            title = i18n("Data Set %1", dataSet->number());
        }
        m_ui.dataSets->addItem(title);
    }
    blockSignals(false);

    dataSetSelectionChanged(index < 0 ? 0 : index);
}
void BubbleConfigWidget::deleteSubDialogs(ChartType type)
{
    Q_UNUSED(type);
    QList<QColorDialog*> lst = findChildren<QColorDialog*>();
    qInfo()<<Q_FUNC_INFO<<lst;
    for (QColorDialog *dlg : lst) {
        dlg->reject();
    }
}

void BubbleConfigWidget::dataSetSelectionChanged(int index)
{
    // Check for valid index
    qInfo()<<Q_FUNC_INFO<<index;
    DataSet *ds = m_dataSets.value(index);
    if (!ds) {
        return;
    }
    blockSignals(true);

    m_ui.dataSetBrush->setColor(ds->brush(index).color());

    m_ui.dataSetPen->setColor(ds->pen(index).color());

    m_ui.dataSetShowCategory->setChecked(ds->valueLabelType(index).category);

    m_ui.dataSetShowNumber->setChecked(ds->valueLabelType(index).number);

    m_ui.dataSetShowPercent->setChecked(ds->valueLabelType(index).percentage);

    m_ui.dataSetShowSymbol->setChecked(ds->valueLabelType(index).symbol);

    m_ui.dataSets->setCurrentIndex(index);

    blockSignals(false);

}

void BubbleConfigWidget::brushChanged(const QColor& color)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit brushChanged(ds, color, -1);
}

void BubbleConfigWidget::penChanged(const QColor& color)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit penChanged(ds, color, -1);
}

void BubbleConfigWidget::showCategoryChanged(bool b)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit showCategoryChanged(ds, b, -1);
}

void BubbleConfigWidget::showNumberChanged(bool b)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit showNumberChanged(ds, b, -1);
}

void BubbleConfigWidget::showPercentChanged(bool b)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit showPercentChanged(ds, b, -1);
}

void BubbleConfigWidget::showSymbolChanged(bool b)
{
    DataSet *ds = m_dataSets.value(m_ui.dataSets->currentIndex());
    if (!ds) {
        return;
    }
    emit showSymbolChanged(ds, b, -1);
}
