/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_LEGENDCONFIGWIDGET
#define KOCHART_LEGENDCONFIGWIDGET

#include "ChartShape.h"
#include "ConfigWidgetBase.h"

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

    QAction *createAction();

    /// reimplemented
    virtual bool showOnShapeCreate() override
    {
        return true;
    }

public Q_SLOTS:
    void updateData() override;

    void setLegendOrientation(int boxEntryIndex);
    void setLegendAlignment(int boxEntryIndex);
    void setLegendPosition(int buttonGroupIndex);
    // void setLegendShowTitle(bool toggled);

Q_SIGNALS:
    void showLegendChanged(bool b);
    void legendTitleChanged(const QString &);
    void legendFontChanged(const QFont &font);
    void legendTitleFontChanged(const QFont &font);
    void legendFontSizeChanged(int size);
    void legendSpacingChanged(int spacing);
    void legendShowLinesToggled(bool toggled);
    void legendOrientationChanged(Qt::Orientation orientation);
    void legendAlignmentChanged(Qt::Alignment alignment);
    void legendPositionChanged(KoChart::Position position);

private Q_SLOTS:
    void ui_legendEditFontButtonClicked();

private:
    void createActions();

    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KOCHART_LEGENDCONFIGWIDGET
