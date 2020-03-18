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


#ifndef KOCHART_TITLESCONFIGWIDGET
#define KOCHART_TITLESCONFIGWIDGET


#include "ConfigWidgetBase.h"
#include "ChartShape.h"
#include "ui_TitlesConfigWidget.h"


class KoShape;
class QAction;

namespace KChart
{
    class Position;
    class CartesianAxis;
}

namespace KoChart
{
class ChartShape;
class ChartTool;

/**
 * Titles configuration widget.
 */
class TitlesConfigWidget : public ConfigWidgetBase
{
    Q_OBJECT

public:
    TitlesConfigWidget();
    ~TitlesConfigWidget();

    QAction *createAction();

    /// reimplemented
    bool showOnShapeCreate() override { return true; }

public Q_SLOTS:
    void updateData() override;

private:
    friend ChartTool;
    Ui::TitlesConfigWidget  ui;
};

}  // namespace KoChart


#endif // KOCHART_TITLESCONFIGWIDGET
