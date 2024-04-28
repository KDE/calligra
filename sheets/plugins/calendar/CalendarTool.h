/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CALENDAR_TOOL
#define CALLIGRA_SHEETS_CALENDAR_TOOL

#include <part/CellTool.h>

namespace Calligra
{
namespace Sheets
{

/**
 * @class CalendarTool
 * @brief Class for inserting a calendar into a spreadsheet.
 */
class CalendarTool : public CellTool
{
    Q_OBJECT
public:
    explicit CalendarTool(KoCanvasBase *canvas);
    ~CalendarTool() override;

public Q_SLOTS:
    void activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

    /**
     * This actually inserts the calendar. It reads the configuration
     * from the insert calendar dialog and builds an calendar in the
     * spreadsheet accordingly.
     */
    void insertCalendar(const QDate &start, const QDate &end);

private:
    QList<QPointer<QWidget>> createOptionWidgets() override;
    void setText(Sheet *sheet, int row, int column, const QString &text, bool asString = false);

private:
    Q_DISABLE_COPY(CalendarTool)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CALENDAR_TOOL
