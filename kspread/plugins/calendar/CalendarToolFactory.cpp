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

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

K_PLUGIN_FACTORY(CalendarToolPluginFactory,
                 registerPlugin<CalendarToolFactory>();
                )
K_EXPORT_PLUGIN(CalendarToolPluginFactory("CalendarTool"))


CalendarToolFactory::CalendarToolFactory(QObject *, const QVariantList&)
        : CellToolFactory(KSPREAD_CALENDAR_TOOL_ID)
{
    setToolTip(i18n("Calendar Tool"));
}

CalendarToolFactory::~CalendarToolFactory()
{
}

KoToolBase* CalendarToolFactory::createTool(KoCanvasBase* canvas)
{
    return new CalendarTool(canvas);
}
