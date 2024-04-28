/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CHART_DATABASE_SELECTOR
#define CALLIGRA_SHEETS_CHART_DATABASE_SELECTOR

#include <KoShapeConfigWidgetBase.h>

namespace Calligra
{
namespace Sheets
{
class Map;

/// A widget that is shown for selecting the cell region providding the chart data
class ChartDatabaseSelector : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit ChartDatabaseSelector(Map *map);
    ~ChartDatabaseSelector() override;

    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;

    /// reimplemented
    bool showOnShapeCreate() override
    {
        return true;
    }

protected:
    void showEvent(QShowEvent *event) override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CHART_DATABASE_SELECTOR
