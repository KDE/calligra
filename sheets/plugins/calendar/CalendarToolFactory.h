/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CALENDAR_TOOL_FACTORY
#define CALLIGRA_SHEETS_CALENDAR_TOOL_FACTORY

#include <QObject>
#include <QVariantList>
#include <part/CellToolFactory.h>

#define CALLIGRA_SHEETS_CALENDAR_TOOL_ID "KSpreadCalendarToolId"

namespace Calligra
{
namespace Sheets
{

class CalendarToolFactory : public QObject, public CellToolFactory
{
    Q_OBJECT
public:
    explicit CalendarToolFactory(QObject *parent, const QVariantList &args);
    ~CalendarToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CALENDAR_TOOL_FACTORY
