/***************************************************************************
 *   Copyright (C) 2005 by Raphael Langerhorst <raphael-langerhorst@gmx.at>*
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

#include <koffice_export.h>

#include <QDateTime>

class KAboutData;

namespace KSpread
{
class View;

class InsertCalendarDialog;

/**
 * \class PluginInsertCalender kspread_plugininsertcalendar.h
 * \brief Plugin for inserting a calendar into a spreadsheet.
 * @author Raphael Langerhorst
 *
 * The plugin class for the Insert Calendar plugin.
 * This plugin is designed to work in KSpread and
 * makes it possible to insert calendars into
 * the spreadsheet.
 */
class KSPREAD_EXPORT PluginInsertCalendar : public KParts::Plugin
{
  Q_OBJECT

  protected:

    /**
     * This is the parent part of the plugin. It is used
     * to access the spreadsheet and actually insert the
     * calendar.
     *
     * It is set in the constructor.
     */
    View* m_kspreadView;

    /**
     * This is the dialog used to select the start/end dates
     * of the calendar. It is the user interface of this
     * plugin.
     * @see InsertCalendarDialog, slotShowDialog, slotInsertCalendar
     */
    InsertCalendarDialog* m_dialog;

  public:

    /**
     * Constructor. This constructor is usable with KGenericFactory.
     */
    PluginInsertCalendar( QObject *parent, const QStringList& args );

    /**
     * Virtual destructor.
     */
    virtual ~PluginInsertCalendar();

  public:

    /**
     * Creates about data for the Insert Calendar plugin.
     */
    static KAboutData* createAboutData();

  public slots:

     /**
      * This is called from the plugin action, it shows a dialog that
      * allows start and end date selection and various other
      * options. The calendar can then be inserted into a spreadsheet.
      */
     void slotShowDialog();

     /**
      * This actually inserts the calendar. It reads the configuration
      * from the insert calendar dialog and builds an calendar in the
      * spreadsheet accordingly.
      */
     void slotInsertCalendar(const QDate &start, const QDate &end);
};

}

#endif
