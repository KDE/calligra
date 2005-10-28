/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTCALENDARLISTDIALOG_H
#define KPTCALENDARLISTDIALOG_H

#include "kptcalendarlistdialogbase.h"
#include "kptcalendaredit.h"
#include "kptcalendar.h"

#include <kdialogbase.h>

#include <qstring.h>

class KMacroCommand;

namespace KPlato
{

class CalendarListViewItem;
class KPTProject;
class KPTPart;

class KPTCalendarListDialogImpl : public KPTCalendarListDialogBase {
    Q_OBJECT
public:
    KPTCalendarListDialogImpl(KPTProject &project, QWidget *parent);

    void setBaseCalendars();
    
    KPTCalendar *getCalendar() { return calendar->getCalendar(); }
    void setCalendar(KPTCalendar *cal);

    QPtrList<CalendarListViewItem> &deletedItems();

public slots:    
    void slotSelectionChanged(QListViewItem *listItem);

private slots:
    void slotCheckAllFieldsFilled();
    void slotBaseCalendarActivated(int id);
    void slotCalendarModified();
    void slotDeleteClicked();
    void slotAddClicked();
    void slotEnableButtonOk(bool on);

signals:
    void obligatedFieldsFilled(bool yes);
    void calendarModified();
    void calendarChanged();
    void enableButtonOk(bool on);

protected:
    CalendarListViewItem *findItem(KPTCalendar *cal);
    
private:
    KPTProject &project;
    QPtrList<CalendarListViewItem> m_deletedItems;
    QPtrList<CalendarListViewItem> baseCalendarList;
};

class KPTCalendarListDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTCalendarListDialog(KPTProject &project, QWidget *parent=0, const char *name=0);
    
    KMacroCommand *buildCommand(KPTPart *part);

protected slots:
    void slotOk();
    
private:
    KPTProject &project;
    KPTCalendarListDialogImpl *dia;
};

}  //KPlato namespace

#endif // KPTCALENDARLISTDIALOG_H
