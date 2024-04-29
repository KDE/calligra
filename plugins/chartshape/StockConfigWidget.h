/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    StockConfigWidget(QWidget *parent = nullptr);
    StockConfigWidget(QList<ChartType> types, QWidget *parent = nullptr);
    ~StockConfigWidget();

    void open(ChartShape *shape) override;

    void updateData(ChartType type, ChartSubtype subtype) override;

public Q_SLOTS:
    void gainClicked(const QColor &color);
    void lossClicked(const QColor &color);

Q_SIGNALS:

private:
    void init();

    PlotArea *m_plotArea;

    Ui::StockConfigWidget m_ui;
};

} // namespace KoChart

#endif // KOCHART_STOCKCONFIGWIDGET_H
