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

#ifndef KPTCALENDAREDIT_H
#define KPTCALENDAREDIT_H

#include "kptcalendareditbase.h"

#include <qptrlist.h>

class KPTCalendar;
class QDate;

class KPTCalendarEdit : public KPTCalendarEditBase {
    Q_OBJECT
public:
    KPTCalendarEdit (QWidget *parent=0, const char *name=0);

    KPTCalendar *getCalendar() { return m_calendar; }
    void setCalendar(KPTCalendar *cal);

    void clear();
    void clearPanel();
    void clearEditPart();
    
private slots:
    void slotCheckAllFieldsFilled();
    void slotDateSelected(QDate date);
    void slotWeekdaySelected(int day);
    void slotWeekSelected(int week, int year);
    void slotStateActivated(int id);
    void slotClearClicked();
    void slotAddIntervalClicked();
    void slotApplyClicked();
    void slotSelectionCleared();

signals:
    void obligatedFieldsFilled(bool yes);
    void applyClicked();

private:
    KPTCalendar *m_calendar;
};

#endif // KPTCALENDAREDIT_H
