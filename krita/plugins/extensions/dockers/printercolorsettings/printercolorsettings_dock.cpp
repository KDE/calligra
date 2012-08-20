/*
 *  Copyright (c) 2012 Joseph Simon <jsimon383@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "printercolorsettings_dock.h"

#include <klocale.h>

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>
#include <stdio.h>
#include <kis_view2.h>
#include "kis_printer_color_manager.h"

PrinterColorSettingsDock::PrinterColorSettingsDock(KisView2 *view)
    : QDockWidget(i18n("Printer Color Settings")),
    m_view(view)
{  
    KisPrinterColorManager *dockWidget = new KisPrinterColorManager(this);  
    setWidget(dockWidget);
}

PrinterColorSettingsDock::~PrinterColorSettingsDock()
{
}

#include "printercolorsettings_dock.moc"
