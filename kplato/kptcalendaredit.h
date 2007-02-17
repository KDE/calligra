/* This file is part of the KDE project
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTCALENDAREDIT_H
#define KPTCALENDAREDIT_H

#include "kptcalendareditbase.h"

#include <q3ptrlist.h>

#include <kdialog.h>

class QDate;

class KCommand;

namespace KPlato
{

class Calendar;
class Part;
class Project;

class CalendarEdit : public CalendarEditBase {
    Q_OBJECT
public:
    CalendarEdit (QWidget *parent=0, const char *name=0);

    Calendar *getCalendar() { return m_calendar; }
    void setCalendar(Calendar *cal, const QString &tz, bool disable);

    void clear();
    void clearPanel();
    void clearEditPart();
    
private slots:
    void slotCheckAllFieldsFilled();
    void slotDateSelected(const QDate& date);
    void slotWeekdaySelected(int day);
    void slotStateActivated(int id);
    void slotClearClicked();
    void slotAddIntervalClicked();
    void slotApplyClicked();
    void slotSelectionCleared();
    void slotTimeZoneChanged( int );

signals:
    void applyClicked();
    void obligatedFieldsFilled(bool yes);
    void changed();
    void timeZoneChanged();

private:
    Calendar *m_calendar;
    bool m_changed;
};

//------------------------------
class CalendarEditDialog : public KDialog
{
    Q_OBJECT
public:
    CalendarEditDialog(Project &project, Calendar *calendar, QWidget *parent=0, const char *name=0);
    ~CalendarEditDialog();
    
    KCommand *buildCommand(Part *part);

protected slots:
    void slotOk();
    
private:
    Project &project;
    Calendar *original;
    Calendar *calendar;
    CalendarEdit *dia;
};


}  //KPlato namespace

#endif // CALENDAREDIT_H
