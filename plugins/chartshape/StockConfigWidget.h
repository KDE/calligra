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


#ifndef KOCHART_STOCKCONFIGWIDGET_H
#define KOCHART_STOCKCONFIGWIDGET_H


#include "ConfigSubWidgetBase.h"
#include "ui_StockConfigWidget.h"


namespace KoChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class StockConfigWidget : public ConfigSubWidgetBase
{
    Q_OBJECT

public:
    StockConfigWidget(QWidget *parent = 0);
    StockConfigWidget(QList<ChartType> types, QWidget *parent = 0);
    ~StockConfigWidget();

    void open(ChartShape *shape) override;

    void updateData(ChartType type, ChartSubtype subtype) override;

public Q_SLOTS:
    void gainClicked(const QColor& color);
    void lossClicked(const QColor& color);

Q_SIGNALS:

private:
    void init();

    PlotArea *m_plotArea;

    Ui::StockConfigWidget m_ui;
};

}  // namespace KoChart


#endif // KOCHART_STOCKCONFIGWIDGET_H
