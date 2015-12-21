/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "CalendarToolFactory.h"

#include "CalendarTool.h"

#include <kpluginfactory.h>
#include <KLocalizedString>

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(CalendarToolPluginFactory, "kspread_plugin_tool_calendar.json",
                           registerPlugin<CalendarToolFactory>();)


CalendarToolFactory::CalendarToolFactory(QObject *, const QVariantList&)
        : CellToolFactory(CALLIGRA_SHEETS_CALENDAR_TOOL_ID)
{
    setToolTip(i18n("Calendar"));
}

CalendarToolFactory::~CalendarToolFactory()
{
}

KoToolBase* CalendarToolFactory::createTool(KoCanvasBase* canvas)
{
    return new CalendarTool(canvas);
}

#include "CalendarToolFactory.moc"
