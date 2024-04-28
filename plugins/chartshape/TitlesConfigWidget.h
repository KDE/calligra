/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_TITLESCONFIGWIDGET
#define KOCHART_TITLESCONFIGWIDGET

#include "ChartShape.h"
#include "ConfigWidgetBase.h"
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
    bool showOnShapeCreate() override
    {
        return true;
    }

public Q_SLOTS:
    void updateData() override;

private:
    friend ChartTool;
    Ui::TitlesConfigWidget ui;
};

} // namespace KoChart

#endif // KOCHART_TITLESCONFIGWIDGET
