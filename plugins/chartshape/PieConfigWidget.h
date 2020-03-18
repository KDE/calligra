/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2008 Inge Wallin    <inge@lysator.liu.se>
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
 * Boston, MA 02110-1301, USA.
*/


#ifndef KOCHART_PIECONFIGWIDGET
#define KOCHART_PIECONFIGWIDGET


#include "ConfigSubWidgetBase.h"
#include "ui_PieConfigWidget.h"


namespace KoChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class PieConfigWidget : public ConfigSubWidgetBase
{
    Q_OBJECT

public:
    PieConfigWidget(QWidget *parent = 0);
    PieConfigWidget(QList<ChartType> types, QWidget *parent = 0);
    ~PieConfigWidget();

    void open(ChartShape *shape) override;

    void updateData(ChartType type, ChartSubtype subtype) override;

public Q_SLOTS:
    void dataPointSelectionChanged(int index);
    void brushChanged(const QColor& color);
    void penChanged(const QColor& color);
    void showCategoryChanged(bool b);
    void showNumberChanged(bool b);
    void showPercentChanged(bool b);
    void explodeFactorChanged(int percent);

Q_SIGNALS:
    void penChanged(DataSet *dataSet, const QColor& color, int section);
    void brushChanged(DataSet *dataSet, const QColor& color, int section);
    void showCategoryChanged(DataSet *dataSet, bool b, int section);
    void showNumberChanged(DataSet *dataSet, bool b, int section);
    void showPercentChanged(DataSet *dataSet, bool b, int section);
    void showSymbolChanged(DataSet *dataSet, bool b, int section);
    void explodeFactorChanged(DataSet *dataSet, int section, int percent);

private:
    void init();

    DataSet *m_dataSet;
    Ui::PieConfigWidget m_ui;
};

}  // namespace KoChart


#endif // KOCHART_PIECONFIGWIDGET
