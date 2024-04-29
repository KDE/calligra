/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_RINGCONFIGWIDGET_H
#define KOCHART_RINGCONFIGWIDGET_H

#include "ConfigSubWidgetBase.h"
#include "ui_RingConfigWidget.h"

namespace KoChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class RingConfigWidget : public ConfigSubWidgetBase
{
    Q_OBJECT

public:
    RingConfigWidget(QWidget *parent = nullptr);
    RingConfigWidget(QList<ChartType> types, QWidget *parent = nullptr);
    ~RingConfigWidget();

    void open(ChartShape *shape) override;

    void updateData(ChartType type, ChartSubtype subtype) override;

public Q_SLOTS:
    void categorySelectionChanged(int index);
    void dataSetSelectionChanged(int index);
    void brushChanged(const QColor &color);
    void penChanged(const QColor &color);
    void showCategoryChanged(bool b);
    void showNumberChanged(bool b);
    void showPercentChanged(bool b);
    void explodeFactorChanged(int percent);

Q_SIGNALS:
    void penChanged(KoChart::DataSet *dataSet, const QColor &color, int section);
    void brushChanged(KoChart::DataSet *dataSet, const QColor &color, int section);
    void showCategoryChanged(KoChart::DataSet *dataSet, bool b, int section);
    void showNumberChanged(KoChart::DataSet *dataSet, bool b, int section);
    void showPercentChanged(KoChart::DataSet *dataSet, bool b, int section);
    void explodeFactorChanged(KoChart::DataSet *dataSet, int section, int percent);

private:
    void init();

    QList<DataSet *> m_dataSets;
    Ui::RingConfigWidget m_ui;
};

} // namespace KoChart

#endif // KOCHART_RINGCONFIGWIDGET_H
