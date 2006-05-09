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

#include "kspread_insertcalendardialog.h"

#include "../../kspread_view.h"
#include "../../kspread_doc.h"
#include "../../selection.h"
#include "../../kspread_sheet.h"

#include <kcalendarsystem.h>
#include <kcalendarsystemfactory.h>
#include <kaboutdata.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kofficeversion.h>
#include <kmessagebox.h>

#include <qpoint.h>
#include <qrect.h>

namespace KSpread
{

// make the plugin available
typedef KGenericFactory<PluginInsertCalendar> InsertCalendarFactory;
K_EXPORT_COMPONENT_FACTORY( libkspreadinsertcalendar,  InsertCalendarFactory("kspreadinsertcalendar"))


PluginInsertCalendar::PluginInsertCalendar( QObject *parent, const QStringList& /*args*/ )
  : Plugin(parent)
{
    this->m_kspreadView = 0;
    if (parent)
    {
      if (parent->inherits("KSpread::View"))
      {
        this->m_kspreadView = (View*)parent;
      }
      else
      {
        kWarning() << "Parent does not inherit View!!!" << endl;
      }
    }
    else
    {
      kWarning() << "Plugin created without a parent!!!" << endl;
    }


    this->m_dialog = 0;

    KAction* insert = new KAction(i18n("Insert Calendar..."),
                                  actionCollection(), "kspreadinsertcalendar");
    connect( insert, SIGNAL(triggered(bool)), this, SLOT(slotShowDialog()) );
}

PluginInsertCalendar::~PluginInsertCalendar()
{
}

KAboutData* PluginInsertCalendar::createAboutData()
{
  KAboutData * aboutData = new KAboutData(
    "kspreadinsertcalendar",  //app name
    I18N_NOOP("Insert Calendar"),  //program name
    KOFFICE_VERSION_STRING,  //version
    I18N_NOOP("KSpread Insert Calendar Plugin"),  //short description
    KAboutData::License_BSD,  //license type
    I18N_NOOP("(c) 2005, The KSpread Team"),  //copyright
    0,//I18N_NOOP("The Insert Calendar plugin can be used in spreadsheets"),  //text
    "http://www.koffice.org/kspread/");
  aboutData->addAuthor("Raphael Langerhorst", 0, "Raphael.Langerhorst@kdemail.net");

  return aboutData;
}

void PluginInsertCalendar::slotShowDialog()
{
  kDebug() << "slotShowDialog..." << endl;

  if (this->m_dialog == 0)
  {
    this->m_dialog = new InsertCalendarDialog();

    Q_ASSERT(m_dialog);

    connect(m_dialog,SIGNAL(insertCalendar(const QDate&, const QDate&)),
            this,SLOT(slotInsertCalendar(const QDate&, const QDate&)));
  }

  //@todo if anyone knows a better way to get a background window to foreground, please change this...
  m_dialog->hide();
  m_dialog->show();

}

void PluginInsertCalendar::slotInsertCalendar(const QDate &start, const QDate &end)
{
  //@todo implement
  kDebug() << "slotInsert... still to be implemented" << endl;

  Doc* document = m_kspreadView->doc();

  if (!document)
  {
    KMessageBox::error(0,i18n("Can't insert calendar because no document is set!"),i18n("Error"));
    return;
  }

  if (end < start)
  {
    KMessageBox::error(0,i18n("End date is before start date! Please make sure that end date comes after start date."),i18n("Error"));
    return;
  }

  if (start.daysTo(end) > 3652)
  {
    KMessageBox::error(0,i18n("Calendars shouldn't be longer than 10 years. If you really need such long periods you need to split them up."),i18n("Error"));
    return;
  }

  if (start == end)
  {
    if (KMessageBox::No == KMessageBox::warningYesNo(0,i18n("Start and end dates are equal! Only one day will be inserted, do you want to continue?"),i18n("Warning")))
      return;
  }

  if (start.daysTo(end)> 366)
  {
     if (KMessageBox::No == KMessageBox::warningYesNo(0,i18n("Creating a calendar for a longer period than a year can take up a lot of space, do you want to continue?"),i18n("Warning")))
      return;
  }

  Selection* selection_info = m_kspreadView->selectionInfo();

  Q_ASSERT(selection_info);

  QPoint selection = selection_info->selection().topLeft();

  Sheet* sheet = m_kspreadView->activeSheet();

  Q_ASSERT(sheet);

  if (!sheet)
    return;

  //now let's check if the area is really empty...
  //we use two columns per day and one column for the week number
  int sizeX = 15;
  //we use two rows per week, some additional space between months...
  //so that should be ok, but can be improved of course
  //@todo improve calendar size prediction!
  int sizeY = 4 + (int)(0.5*(float)(start.daysTo(end)));

  if (!sheet->areaIsEmpty(QRect(selection,QSize(sizeX,sizeY))))
  {
    if (KMessageBox::No == KMessageBox::warningYesNo(0,i18n("The area where the calendar is inserted is NOT empty, are you sure you want to continue, overwriting existing data? If you choose No the area that would be required for the desired calendar will be selected so you can see what data would be overwritten."),i18n("Warning")))
    {
      //select the area so the user knows what's in the way
      selection_info->initialize(QRect(selection.x(),selection.y(),sizeX,sizeY));//,sheet);
      return;
    }
  }

  KCalendarSystem* cs = KCalendarSystemFactory::create();

  Q_ASSERT(cs);

  document->emitBeginOperation();

  int row = selection.y();
  int col = selection.x();
  int colstart = col; //this is where we get back after each week
  sheet->setText(row,colstart,i18n("Calendar from %1 to %2").arg(start.toString()).arg(end.toString()));

  QDate current(start);
//   QDate previous(current);
  bool yearheader = true;
  bool monthheader = true;
  bool weekheader = true;

  //this loop creates the actual calendar
  //@todo formatting of cells - each day occupies QRect(row,col,row,col+1)
  while (current <= end)
  {

    //let's see if any header is required
    if (cs->dayOfWeek(current)==1)
    {
      col=colstart;
      row++;
      weekheader=true;
    }
    if (cs->day(current)==1)
    {
      row+=2;
      col=colstart + (cs->dayOfWeek(current)-1)*2;
      monthheader=true;
      weekheader=true;
      if (cs->month(current)==1)
      {
        row++;
        yearheader=true;
      }
    }

    if (yearheader)
    {
      kDebug() << "inserting year " + QString::number(current.year()) << endl;
      sheet->setText(row,colstart+6,cs->yearString(current,false));

      row+=2;
      yearheader=false;
    }
    if (monthheader)
    {
      kDebug() << "inserting month " + QString::number(current.month()) << endl;
      sheet->setText(row,colstart+6,cs->monthName(current,false));
      row+=2;
      //we always have the week number in the first column
      sheet->setText(row,colstart,i18n("week"));
      for (int i=1; i<8; i++)
      {
        sheet->setText(row,colstart+(i-1)*2+1,cs->weekDayName(i));
      }
      row++;
      monthheader=false;
    }
    if (weekheader)
    {
      sheet->setText(row,colstart,QString::number(cs->weekNumber(current)));
      col++;
      weekheader=false;

      //if we are at the beginning of the month we might need an offset
      if (cs->day(current)==1)
      {
        col=colstart + (cs->dayOfWeek(current)-1)*2 + 1;
      }
    }

    sheet->setText(row,col,QString::number(cs->day(current)));
    //go to the next date
    //@todo isn't there a better way, like current++ or something??
    QDate next = current.addDays(1);
    current.setYMD(next.year(),next.month(),next.day());
    col+=2;

  }

  document->emitEndOperation();

  kDebug() << "inserting calendar completed" << endl;
}

}

#include "kspread_plugininsertcalendar.moc"
