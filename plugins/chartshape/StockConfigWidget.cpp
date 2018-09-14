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
#include "StockConfigWidget.h"
#include "ui_StockConfigWidget.h"

// Qt
#include <QLatin1String>

// KF5
#include <klocalizedstring.h>

// Calligra
#include <interfaces/KoChartModel.h>
#include <KoIcon.h>

// KChart

// KoChart
#include "ChartProxyModel.h"
#include "PlotArea.h"
#include "ChartDebug.h"

using namespace KoChart;


StockConfigWidget::StockConfigWidget(QWidget *parent)
    : ConfigSubWidgetBase(parent)
{
    init();
}

StockConfigWidget::StockConfigWidget(QList<ChartType> types, QWidget *parent)
    : ConfigSubWidgetBase(types, parent)
{
    init();
}

StockConfigWidget::~StockConfigWidget()
{
}

void StockConfigWidget::init()
{
    setObjectName("StockConfigWidget");
    m_ui.setupUi(this);
    m_plotArea = 0;

    connect(m_ui.gainMarker, SIGNAL(changed(QColor)), this, SLOT(gainClicked(QColor)));
    connect(m_ui.lossMarker, SIGNAL(changed(QColor)), this, SLOT(lossClicked(QColor)));
}

void StockConfigWidget::open(ChartShape* shape)
{
    ConfigSubWidgetBase::open(shape);
    m_plotArea = 0;
    if (!chart) {
        return;
    }
    m_plotArea = chart->plotArea();
    m_ui.rangeLineStroke->open(shape);
}

void StockConfigWidget::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(subtype);

    if (!chart || !chartTypes.contains(type)) {
        return;
    }
    m_ui.rangeLineStroke->updateData();

    m_ui.gainMarker->setColor(m_plotArea->stockGainBrush().color());
    m_ui.lossMarker->setColor(m_plotArea->stockLossBrush().color());
}

void StockConfigWidget::gainClicked(const QColor& color)
{
    QBrush brush = QBrush(color);
    brush.setStyle(Qt::SolidPattern);
    m_plotArea->setStockGainBrush(brush);
    chart->updateAll();
}

void StockConfigWidget::lossClicked(const QColor& color)
{
    QBrush brush = QBrush(color);
    brush.setStyle(Qt::SolidPattern);
    m_plotArea->setStockLossBrush(brush);
    chart->updateAll();
}
