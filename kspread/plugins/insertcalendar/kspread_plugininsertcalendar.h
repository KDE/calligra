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

#ifndef KSPREAD_PLUGININSERTCALENDAR_H
#define KSPREAD_PLUGININSERTCALENDAR_H
 
#include <kparts/plugin.h>

namespace KSpread
{

class InsertCalendarSettings;

/**
 * The plugin class for the Insert Calendar plugin.
 * This plugin is design to work in KSpread and
 * makes it possible to insert calendars into
 * the spreadsheet.
 */
class PluginInsertCalendar : public KParts::Plugin
{
  Q_OBJECT
  
  protected:
  
    /**
     * All settings that are made in the dialog are stored
     * here, when inserting a calendar these settings are used.
     * @see slotInsert
     */
    InsertCalendarSettings* m_pSettings;
  public:
    
    /**
     * Constructor.
     */
    PluginInsertCalendar( QObject* parent = 0, const char* name = 0 );
    
    /**
     * virtual destructor.
     */
     virtual ~PluginInsertCalendar();
 
  public slots:
 
     /**
      * This is called from the plugin action, it shows a dialog that
      * allows start and end date selection and various other
      * options. The calendar can then be inserted into a spreadsheet.
      */
     void slotShowDialog();
     
     /**
      * This actually inserts the calendar. It reads the configuration
      * from the settings and builds an calendar in the spreadsheet
      * accordingly.
      */
     void slotInsert();
};

}

#endif
