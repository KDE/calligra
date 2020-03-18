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


#ifndef KOCHART_LEGENDCONFIGWIDGET
#define KOCHART_LEGENDCONFIGWIDGET


#include "ConfigWidgetBase.h"
#include "ChartShape.h"


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

/**
 * Chart type configuration widget.
 */
class LegendConfigWidget : public ConfigWidgetBase
{
    Q_OBJECT

public:
    LegendConfigWidget();
    ~LegendConfigWidget();

    QAction * createAction();

    /// reimplemented
    virtual bool showOnShapeCreate() override { return true; }

public Q_SLOTS:
    void updateData() override;

    void setLegendOrientation(int boxEntryIndex);
    void setLegendAlignment(int boxEntryIndex);
    void setLegendPosition(int buttonGroupIndex);
    //void setLegendShowTitle(bool toggled);

Q_SIGNALS:
    void showLegendChanged(bool b);
    void legendTitleChanged(const QString&);
    void legendFontChanged(const QFont& font);
    void legendTitleFontChanged(const QFont& font);
    void legendFontSizeChanged(int size);
    void legendSpacingChanged(int spacing);
    void legendShowLinesToggled(bool toggled);
    void legendOrientationChanged(Qt::Orientation orientation);
    void legendAlignmentChanged(Qt::Alignment alignment);
    void legendPositionChanged(Position position);

private Q_SLOTS:
    void ui_legendEditFontButtonClicked();

private:
    void createActions();

    class Private;
    Private * const d;
};

}  // namespace KoChart


#endif // KOCHART_LEGENDCONFIGWIDGET
