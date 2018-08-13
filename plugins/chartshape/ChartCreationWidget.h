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


#ifndef KOCHART_CHARTCREATIONWIDGET_H
#define KOCHART_CHARTCREATIONWIDGET_H

#include <KoShapeConfigWidgetBase.h>

#include "ui_ChartCreationWidget.h"
#include "kochart_global.h"

class KoShape;

namespace KoChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class ChartCreationWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    ChartCreationWidget();
    ~ChartCreationWidget();

    virtual bool showOnShapeCreate();

    /**
     * Open the argument shape by interpreting the data and setting that data on this
     * widget.
     * @param shape the shape that is to be queried for the data this widget can edit.
     */
    virtual void open(KoShape *shape);
    /**
     * Save the data  of this widget to the shape passed to open earlier to
     * apply any user changed options.
     * Called by the tool that created the shape.
     */
    virtual void save();

protected Q_SLOTS:
    void toggled(bool);

private:
    void updateTypes(const QToolButton *btn);
    void createData();
    void createStockData();
    void createRadarData();

private:
    ChartShape *m_chart;
    ChartType m_type;
    ChartSubtype m_subType;
    Ui::ChartCreationWidget m_ui;
};

}  // namespace KoChart


#endif // KOCHART_CHARTCREATIONWIDGET_H
