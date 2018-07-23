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


#ifndef KOCHART_BUBBLECONFIGWIDGET_H
#define KOCHART_BUBBLECONFIGWIDGET_H


#include "ConfigObjectBase.h"
#include "ui_BubbleConfigWidget.h"


namespace KoChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class BubbleConfigWidget : public ConfigObjectBase
{
    Q_OBJECT

public:
    BubbleConfigWidget(QWidget *parent = 0);
    BubbleConfigWidget(QList<ChartType> types, QWidget *parent = 0);
    ~BubbleConfigWidget();

    void open(ChartShape *shape);

    void updateData(ChartType type, ChartSubtype subtype);

    void deleteSubDialogs(ChartType type = LastChartType);

public Q_SLOTS:
    void dataSetSelectionChanged(int index);
    void brushChanged(const QColor& color);
    void penChanged(const QColor& color);
    void showCategoryChanged(bool b);
    void showNumberChanged(bool b);
    void showPercentChanged(bool b);
    void showSymbolChanged(bool b);

Q_SIGNALS:
    void penChanged(DataSet *dataSet, const QColor& color, int section);
    void brushChanged(DataSet *dataSet, const QColor& color, int section);
    void showCategoryChanged(DataSet *dataSet, bool b, int section);
    void showNumberChanged(DataSet *dataSet, bool b, int section);
    void showPercentChanged(DataSet *dataSet, bool b, int section);
    void showSymbolChanged(DataSet *dataSet, bool b, int section);

private:
    void init();

    QList<DataSet*> m_dataSets;
    Ui::BubbleConfigWidget m_ui;
};

}  // namespace KoChart


#endif // KOCHART_BUBBLECONFIGWIDGET_H
