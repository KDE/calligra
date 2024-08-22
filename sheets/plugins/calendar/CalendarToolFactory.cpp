/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CalendarToolFactory.h"

#include "CalendarTool.h"

#include <KLocalizedString>
#include <KPluginFactory>

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(CalendarToolPluginFactory, "kspread_plugin_tool_calendar.json", registerPlugin<CalendarToolFactory>();)

CalendarToolFactory::CalendarToolFactory(QObject *, const QVariantList &)
    : CellToolFactory(CALLIGRA_SHEETS_CALENDAR_TOOL_ID)
{
    setToolTip(i18n("Calendar"));
}

CalendarToolFactory::~CalendarToolFactory() = default;

KoToolBase *CalendarToolFactory::createTool(KoCanvasBase *canvas)
{
    return new CalendarTool(canvas);
}

#include "CalendarToolFactory.moc"
