/***************************************************************************
 *   Copyright (C) 2005 by Raphael Langerhorst                             *
 *   raphael-langerhorst@gmx.at                                            *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

#include "kspread_plugininsertcalendar.h"

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

namespace KSpread
{

// make the plugin available
typedef KGenericFactory<PluginInsertCalendar> InsertCalendarFactory;
K_EXPORT_COMPONENT_FACTORY( libkspreadinsertcalendar,  InsertCalendarFactory("kspread"))

/**
 * \class InsertCalendarSettings kspread_plugininsertcalendar.cc
 * \brief Holds all settings for the insertion (start/end dates, ...).
 * @author Raphael Langerhorst
 */
class InsertCalendarSettings
{
  int start, end;
};

static const char* description=I18N_NOOP("KOffice Spreadsheet Application");

// Always the same as the KOffice version
static const char* version=KOFFICE_VERSION_STRING;

PluginInsertCalendar::PluginInsertCalendar( QObject *parent, const char *name, const QStringList& args )
: Plugin(parent,name)
{
    this->m_pSettings = new InsertCalendarSettings();
    
    (void)new KAction( i18n("Insert Calendar"), KShortcut::null(),
                   this, SLOT( slotShowDialog() ), actionCollection(), "kspreadinsertcalendar");
}

PluginInsertCalendar::~PluginInsertCalendar()
{
}
     
KAboutData* PluginInsertCalendar::createAboutData()
{
  KAboutData * aboutData = new KAboutData( "kspreadinsertcalendar", I18N_NOOP("Insert Calendar"),
                   KOFFICE_VERSION_STRING, I18N_NOOP("KSpread Insert Calendar Plugin"),
                   KAboutData::License_BSD, I18N_NOOP("(c) 2005, The KSpread Team"), 0,
                   "http://www.koffice.org/kspread/");
  aboutData->addAuthor("Raphael Langerhorst", 0, "raphael-langerhorst@gmx.at");
  
  return aboutData;
}
 
void PluginInsertCalendar::slotShowDialog()
{
  //@todo implement
  kdDebug() << "slotShowDialog..." << endl;
}

void PluginInsertCalendar::slotInsert()
{
  //@todo implement
  kdDebug() << "slotInsert..." << endl;
}

}
