/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "StockConfigWidget.h"
#include "ui_StockConfigWidget.h"

// Qt
#include <QLatin1String>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoIcon.h>
#include <interfaces/KoChartModel.h>

// KChart

// KoChart
#include "ChartDebug.h"
#include "ChartProxyModel.h"
#include "PlotArea.h"

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

StockConfigWidget::~StockConfigWidget() = default;

void StockConfigWidget::init()
{
    setObjectName("StockConfigWidget");
    m_ui.setupUi(this);
    m_plotArea = nullptr;

    connect(m_ui.gainMarker, &KColorButton::changed, this, &StockConfigWidget::gainClicked);
    connect(m_ui.lossMarker, &KColorButton::changed, this, &StockConfigWidget::lossClicked);
}

void StockConfigWidget::open(ChartShape *shape)
{
    ConfigSubWidgetBase::open(shape);
    m_plotArea = nullptr;
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

void StockConfigWidget::gainClicked(const QColor &color)
{
    QBrush brush = QBrush(color);
    brush.setStyle(Qt::SolidPattern);
    m_plotArea->setStockGainBrush(brush);
    chart->updateAll();
}

void StockConfigWidget::lossClicked(const QColor &color)
{
    QBrush brush = QBrush(color);
    brush.setStyle(Qt::SolidPattern);
    m_plotArea->setStockLossBrush(brush);
    chart->updateAll();
}
