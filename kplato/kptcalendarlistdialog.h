/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTCALENDARLISTDIALOG_H
#define KPTCALENDARLISTDIALOG_H

#include "kptcalendarlistdialogbase.h"
#include "kptcalendaredit.h"
#include "kptcalendar.h"

#include <kdialogbase.h>

#include <qstring.h>

class KMacroCommand;
class KPTProject;
class CalendarListViewItem;

class KPTCalendarListDialogImpl : public KPTCalendarListDialogBase {
    Q_OBJECT
public:
    KPTCalendarListDialogImpl (QWidget *parent);

    void setBaseCalendars();
    
    KPTCalendar *getCalendar() { return calendar->getCalendar(); }
    void setCalendar(KPTCalendar *cal);

    QPtrList<CalendarListViewItem> &deletedItems();

private slots:
    void slotCheckAllFieldsFilled();
    void slotBaseCalendarActivated(int id);
    void slotSelectionChanged(QListViewItem *listItem);
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
    QPtrList<CalendarListViewItem> m_deletedItems;
    QPtrList<CalendarListViewItem> baseCalendarList;
};

class KPTCalendarListDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTCalendarListDialog(KPTProject &project, QWidget *parent=0, const char *name=0);
    
    KMacroCommand *buildCommand();

protected slots:
    void slotOk();
    
private:
    KPTProject &project;
    KPTCalendarListDialogImpl *dia;
};

#endif // KPTCALENDARLISTDIALOG_H
