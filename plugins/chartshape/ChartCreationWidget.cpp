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

#include "ChartCreationWidget.h"

#include "ChartShape.h"
#include "TableSource.h"
#include "ChartProxyModel.h"
#include "CellRegion.h"
#include "Legend.h"
#include "Axis.h"
#include "PlotArea.h"
#include "ChartLayout.h"
#include "DataSet.h"
#include "ChartDebug.h"

#include <QStandardItemModel>

using namespace KoChart;


ChartCreationWidget::ChartCreationWidget()
    : KoShapeConfigWidgetBase()
    , m_chart(0)
    , m_type(LastChartType)
    , m_subType(NoChartSubtype)
{
    m_ui.setupUi(this);

    connect(m_ui.chartBar, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBarStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBarPercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLine, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLineStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLinePercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartArea, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartAreaStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartAreaPercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartPie, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRing, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockHLC, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockOHLC, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockCandleStick, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRadar, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRadarFilled, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBubbles, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartScatter, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
}


ChartCreationWidget::~ChartCreationWidget()
{
}

bool ChartCreationWidget::showOnShapeCreate()
{
    return true;
}

void ChartCreationWidget::open(KoShape *shape)
{
    m_chart = dynamic_cast<ChartShape*>(shape);
}

void ChartCreationWidget::save()
{
    if (!m_chart) {
        return;
    }
    m_chart->setChartType(m_type);
    m_chart->setChartSubType(m_subType);
    createData();
    m_chart->proxyModel()->reset(m_chart->proxyModel()->cellRangeAddress());
}

void ChartCreationWidget::toggled(bool value)
{
    if (!value) {
        return;
    }
    QToolButton *btn = qobject_cast<QToolButton*>(sender());
    if (!btn) {
        return;
    }
    if (btn == m_ui.chartBar) {
        m_type = BarChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartBarStacked) {
        m_type = BarChartType;
        m_subType = StackedChartSubtype;
    } else if (btn == m_ui.chartBarPercent) {
        m_type = BarChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartLine) {
        m_type = LineChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartLineStacked) {
        m_type = LineChartType;
        m_subType = StackedChartSubtype;
    } else if (btn == m_ui.chartLinePercent) {
        m_type = LineChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartArea) {
        m_type = AreaChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartAreaStacked) {
        m_type = AreaChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartAreaPercent) {
        m_type = AreaChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartPie) {
        m_type = CircleChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartRing) {
        m_type = RingChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartStockHLC) {
        m_type = StockChartType;
        m_subType = HighLowCloseChartSubtype;
    } else if (btn == m_ui.chartStockOHLC) {
        m_type = StockChartType;
        m_subType = OpenHighLowCloseChartSubtype;
    } else if (btn == m_ui.chartStockCandleStick) {
        m_type = StockChartType;
        m_subType = CandlestickChartSubtype;
    } else if (btn == m_ui.chartRadar) {
        m_type = RadarChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartRadarFilled) {
        m_type = FilledRadarChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartScatter) {
        m_type = ScatterChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartBubbles) {
        m_type = BubbleChartType;
        m_subType = NoChartSubtype;
    } else {
        Q_ASSERT(true);
    }
}

void ChartCreationWidget::createStockData()
{
    Table *internalTable = 0;
    QStandardItemModel *data = qobject_cast<QStandardItemModel*>(m_chart->internalModel());
    if (data) {
        data->clear();
    } else {
        data = new QStandardItemModel();
        internalTable = m_chart->tableSource()->add("internal-model", data);
        m_chart->setInternalModel(data);
    }

    data->setRowCount(4);
    data->setColumnCount(5);

    // Vertical header data
    data->setData(data->index(1, 0), i18n("Share A"));
    data->setData(data->index(2, 0), i18n("Share B"));
    data->setData(data->index(3, 0), i18n("Share C"));

    // Horizontal header data
    data->setData(data->index(0, 1), i18n("Column %1", 1));
    data->setData(data->index(0, 2), i18n("Column %1", 2));
    data->setData(data->index(0, 3), i18n("Column %1", 3));
    data->setData(data->index(0, 4), i18n("Column %1", 4));

    // First row
    data->setData(data->index(1, 1), 5.7);
    data->setData(data->index(1, 2), 6.5);
    data->setData(data->index(1, 3), 1.2);
    data->setData(data->index(1, 4), 6.0);

    // Second row
    data->setData(data->index(2, 1), 2.1);
    data->setData(data->index(2, 2), 6.5);
    data->setData(data->index(2, 3), 0.9);
    data->setData(data->index(2, 4), 1.5);

    // Third row
    data->setData(data->index(3, 1), 7.9);
    data->setData(data->index(3, 2), 8.5);
    data->setData(data->index(3, 3), 4.6);
    data->setData(data->index(3, 4), 8.3);

    m_chart->legend()->setVisible(false);
    m_chart->plotArea()->xAxis()->setTitleText(QString());
    m_chart->plotArea()->yAxis()->setTitleText(QString());

    ChartProxyModel *proxyModel = m_chart->proxyModel();
    proxyModel->removeRows(0, proxyModel->rowCount());

    proxyModel->setFirstRowIsLabel(true);
    proxyModel->setFirstColumnIsLabel(true);
    proxyModel->setDataDirection(Qt::Vertical);
//     int row = 1;
//     int col = 2;
//     switch (m_subType) {
//         case HighLowCloseChartSubtype:
//             for (DataSet *ds : proxyModel->dataSets()) {
//                 ds->setYDataRegion(CellRegion(internalTable, QRect(3, col, 5, col)));
//                 ds->setCategoryDataRegion(CellRegion(internalTable, QRect(row, 1, row, 1)));
//                 ++row;
//                 ++ col;
//             }
//             break;
//         case OpenHighLowCloseChartSubtype:
//         case CandlestickChartSubtype:
//             for (DataSet *ds : proxyModel->dataSets()) {
//                 ds->setYDataRegion(CellRegion(internalTable, QRect(2, col, 5, col)));
//                 ds->setCategoryDataRegion(CellRegion(internalTable, QRect(row, 1, row, 1)));
//                 ++row;
//                 ++col;
//             }
//             break;
//     }
}

void ChartCreationWidget::createRadarData()
{

}

void ChartCreationWidget::createData()
{
    switch (m_type) {
        case BarChartType:
        case LineChartType:
        case AreaChartType:
        case CircleChartType:
        case RingChartType:
            break;
        case StockChartType:
            createStockData();
            break;
        case RadarChartType:
        case FilledRadarChartType:
            createRadarData();
            break;
        case BubbleChartType:
        case ScatterChartType:
            break;
        default:
            break;

    }
}
