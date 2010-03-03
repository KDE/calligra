/* This file is part of the KDE project
   Copyright (C) 2004 - 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTINTERVALEDIT_H
#define KPTINTERVALEDIT_H

#include "kplatoui_export.h"

#include "ui_kptintervaleditbase.h"
#include "kptcalendar.h"

#include <kdialog.h>

#include <QWidget>

namespace KPlato
{

class MacroCommand;

class IntervalEditBase : public QWidget, public Ui::IntervalEditBase
{
public:
    explicit IntervalEditBase(QWidget *parent) : QWidget( parent ) {
    setupUi( this );
  }
};


class IntervalEditImpl : public IntervalEditBase {
    Q_OBJECT
public:
    IntervalEditImpl(QWidget *parent);
    
    QList<TimeInterval*> intervals() const;
    void setIntervals(const QList<TimeInterval*> &intervals);
    
protected slots:
    void slotClearClicked();
    void slotAddIntervalClicked();
    void slotRemoveIntervalClicked();
    void slotIntervalSelectionChanged();
    void enableButtons();
    
signals:
    void changed();
};

class IntervalEdit : public IntervalEditImpl {
    Q_OBJECT
public:
    explicit IntervalEdit( CalendarDay *day, QWidget *parent=0);

};

class KPLATOUI_EXPORT IntervalEditDialog : public KDialog
{
    Q_OBJECT
public:
    explicit IntervalEditDialog( Calendar *calendar, const QList<CalendarDay*> &days, QWidget *parent = 0 );
    
    explicit IntervalEditDialog( Calendar *calendar, const QList<QDate> &dates, QWidget *parent = 0 );

    MacroCommand *buildCommand();
    QList<TimeInterval*> intervals() const { return m_panel->intervals(); }
    
protected slots:
    void slotChanged();
    void slotCalendarRemoved( const Calendar *cal );

protected:
    MacroCommand *buildCommand( Calendar *calendar, CalendarDay *day );

private:
    Calendar *m_calendar;
    QList<CalendarDay*> m_days;
    QList<QDate> m_dates;
    IntervalEdit *m_panel;
};

}  //KPlato namespace

#endif // INTERVALEDIT_H
