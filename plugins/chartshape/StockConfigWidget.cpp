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
#include <QToolButton>
#include <QColorDialog>

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

    m_ui.gainMarker->setIcon(koIcon("color-picker-black"));
    m_ui.gainMarker->setToolTip(i18nc("@info:tooltip", "Select gain marker color"));

    m_ui.lossMarker->setIcon(koIcon("color-picker-white"));
    m_ui.lossMarker->setToolTip(i18nc("@info:tooltip", "Select loss marker color"));

    m_plotArea = 0;

    connect(m_ui.gainMarker, &QToolButton::clicked, this, &StockConfigWidget::gainClicked);
    connect(m_ui.lossMarker, &QToolButton::clicked, this, &StockConfigWidget::lossClicked);
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

    m_ui.gainMarker->setStyleSheet(QString("QToolButton#gainMarker { background-color: %1 }").arg(m_plotArea->stockGainBrush().color().name()));
    m_ui.lossMarker->setStyleSheet(QString("QToolButton#lossMarker { background-color: %1 }").arg(m_plotArea->stockLossBrush().color().name()));
}

void StockConfigWidget::gainClicked()
{
    QColorDialog dlg(m_plotArea->stockGainBrush().color());
    if (dlg.exec() == QDialog::Accepted) {
        QBrush brush = QBrush(dlg.selectedColor());
        brush.setStyle(Qt::SolidPattern);
        m_plotArea->setStockGainBrush(brush);
        m_plotArea->plotAreaUpdate();
        chart->update();
    }
}

void StockConfigWidget::lossClicked()
{
    QColorDialog dlg(m_plotArea->stockLossBrush().color());
    if (dlg.exec() == QDialog::Accepted) {
        QBrush brush = QBrush(dlg.selectedColor());
        brush.setStyle(Qt::SolidPattern);
        m_plotArea->setStockLossBrush(brush);
        m_plotArea->plotAreaUpdate();
        chart->update();
    }
}
