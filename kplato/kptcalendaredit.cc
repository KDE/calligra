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

#include "kptcalendaredit.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcalendarpanel.h"
#include "kptmap.h"
#include "intervalitem.h"

#include <q3buttongroup.h>
#include <QHeaderView>
#include <QPushButton>
#include <qradiobutton.h>
#include <QComboBox>
#include <QLabel>
#include <q3textedit.h>
#include <QLineEdit>
#include <q3datetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <q3textbrowser.h>
#include <QMap>

#include <k3command.h>
#include <klocale.h>
#include <kdebug.h>
#include <ksystemtimezone.h>
#include <ktimezones.h>

namespace KPlato
{

CalendarEdit::CalendarEdit (QWidget *parent, const char */*name*/)
    : CalendarEditBase(parent),
      m_calendar(0),
      m_changed( false )
 {

    clear();
    intervalList->header()->setStretchLastSection(true);
//     intervalList->setShowSortIndicator(true);
//     intervalList->setSorting(0);

    connect (calendarPanel, SIGNAL(dateChanged(QDate)), SLOT(slotDateSelected(QDate)));
    connect (calendarPanel, SIGNAL(weekdaySelected(int)), SLOT(slotWeekdaySelected(int)));
    connect(calendarPanel, SIGNAL(selectionCleared()), SLOT(slotSelectionCleared()));

    connect (state, SIGNAL(activated(int)), SLOT(slotStateActivated(int)));
    connect (bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect (bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));

    connect (bApply, SIGNAL(clicked()), SLOT(slotApplyClicked()));
    connect (timezone, SIGNAL( activated( int ) ), SLOT( slotTimeZoneChanged( int ) ) );
}

void CalendarEdit::slotTimeZoneChanged( int )
{
    //kDebug()<<k_funcinfo<<endl;
    QStringList lst;
    QString name = timezone->currentText();
    const KTimeZone *tz = 0;
    foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
        if ( name == i18n( s.toUtf8() ) ) {
            if ( s != m_calendar->timeZone()->name() ) {
                tz = KSystemTimeZones::zone( s );
            }
            break;
        }
    }
    if ( tz ) {
        m_calendar->setTimeZone( tz );
        m_changed = true;
        emit timeZoneChanged();
        slotCheckAllFieldsFilled();
    }
}

void CalendarEdit::slotStateActivated(int id) {
    //kDebug()<<k_funcinfo<<"id="<<id<<endl;
    if (id == 0) { // undefined
        startTime->setEnabled(false);
        endTime->setEnabled(false);
        bClear->setEnabled(false);
        bAddInterval->setEnabled(false);
        intervalList->setEnabled(false);
        bApply->setEnabled(true);
    } else if (id == 1) { // non working
        startTime->setEnabled(false);
        endTime->setEnabled(false);
        bClear->setEnabled(false);
        bAddInterval->setEnabled(false);
        intervalList->setEnabled(false);
        bApply->setEnabled(true);
    } else if (id == 2) { //working
        startTime->setEnabled(true);
        endTime->setEnabled(true);
        bClear->setEnabled(true);
        bAddInterval->setEnabled(true);
        intervalList->setEnabled(true);
        bApply->setEnabled(intervalList->topLevelItemCount() > 0);
    }
}

void CalendarEdit::slotClearClicked() {
    //kDebug()<<k_funcinfo<<endl;
    intervalList->clear();
    bApply->setEnabled(false);
}
void CalendarEdit::slotAddIntervalClicked() {
    //kDebug()<<k_funcinfo<<endl;
    intervalList->addTopLevelItem(new IntervalItem(intervalList, startTime->time(), endTime->time()));
    bApply->setEnabled(true);
}

//NOTE: enum CalendarDay::State must match combobox state!
void CalendarEdit::slotApplyClicked() {
    //kDebug()<<k_funcinfo<<"("<<m_calendar<<")"<<endl;
    DateMap dates = calendarPanel->selectedDates();
    for(DateMap::iterator it = dates.begin(); it != dates.end(); ++it) {
        QDate date = QDate::fromString(it.key(), Qt::ISODate);
        //kDebug()<<k_funcinfo<<"Date: "<<date<<endl;
        CalendarDay *calDay = m_calendar->findDay(date);
        if (!calDay) {
            calDay = new CalendarDay(date);
            m_calendar->addDay(calDay);
        }
        calDay->setState(state->currentIndex()); //NOTE!!
        calDay->clearIntervals();
        if (calDay->state() == CalendarDay::Working) {
            int cnt = intervalList->topLevelItemCount();
            for (int i = 0; i < cnt; ++i) {
                //kDebug()<<k_funcinfo<<"Adding interval: "<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString()<<endl;
                calDay->addInterval(static_cast<IntervalItem *>(intervalList->topLevelItem(i))->interval());
            }
        }
    }

    IntMap weekdays = calendarPanel->selectedWeekdays();
    for(IntMap::iterator it = weekdays.begin(); it != weekdays.end(); ++it) {
        //kDebug()<<k_funcinfo<<"weekday="<<it.key()<<endl;
        CalendarDay *weekday = m_calendar->weekday(it.key());
        weekday->setState(state->currentIndex());//NOTE!!
        weekday->clearIntervals();
        if (weekday->state() == CalendarDay::Working) {
            int cnt = intervalList->topLevelItemCount();
            for (int i = 0; i < cnt; ++i) {
                //kDebug()<<k_funcinfo<<"Adding interval: "<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString()<<endl;
                weekday->addInterval(static_cast<IntervalItem *>(intervalList->topLevelItem(i))->interval());
            }
        }
    }

    calendarPanel->markSelected(state->currentIndex()); //NOTE!!
    m_changed = true;
    emit applyClicked();
    slotCheckAllFieldsFilled();
}

void CalendarEdit::slotCheckAllFieldsFilled() {
    //kDebug()<<k_funcinfo<<endl;
    if ( calendarPanel->selectedDates().isEmpty() && calendarPanel->selectedWeekdays().isEmpty() ) {
        emit obligatedFieldsFilled(m_changed);
        return;
    }
    if (state->currentItem() == 0 /*undefined*/ ||
        state->currentIndex() == 1 /*Non-working*/||
        (state->currentItem() == 2 /*Working*/ && intervalList->topLevelItemCount() > 0))
    {
        emit obligatedFieldsFilled(m_changed);
    }
    else if (state->currentIndex() == 2 && !intervalList->topLevelItemCount() > 0)
    {
        emit obligatedFieldsFilled(false);
    }
}

void CalendarEdit::setCalendar(Calendar *cal, const QString &tz, bool disable)
{
    m_calendar = cal;
    clear();
    if ( disable ) {
        timezone->addItem( i18n( tz.toUtf8() ) );
    } else {
        QStringList lst;
        foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
            lst << i18n( s.toUtf8() );
        }
        lst.sort();
        timezone->addItems( lst );
        timezone->setCurrentIndex( lst.indexOf( i18n( tz.toUtf8() ) ) );
    }
    calendarPanel->setCalendar(cal);
}

void CalendarEdit::clear() {
    timezone->clear();
    clearPanel();
    clearEditPart();
}

void CalendarEdit::clearPanel() {
    calendarPanel->clear();
}

void CalendarEdit::clearEditPart() {
    day->setEnabled(true);
    intervalList->clear();
    intervalList->setEnabled(false);
    startTime->setEnabled(false);
    startTime->setTime(QTime(8, 0, 0)); //FIXME
    endTime->setEnabled(false);
    endTime->setTime(QTime(16, 0, 0)); //FIXME

    bAddInterval->setEnabled(false);
    bClear->setEnabled(false);
    bApply->setEnabled(false);
    state->setEnabled(false);
}

void CalendarEdit::slotDateSelected(const QDate& date) {
    if (m_calendar == 0)
        return;
    //kDebug()<<k_funcinfo<<"("<<date.toString()<<")"<<endl;
    clearEditPart();
    state->clear();
    state->addItem(i18n("Undefined"));
    state->addItem(i18n("Non-working"));
    state->addItem(i18n("Working"));

    CalendarDay *calDay = m_calendar->findDay(date);
    state->setEnabled(true);
    if (calDay) {
        foreach (TimeInterval *i, calDay->workingIntervals()) {
            IntervalItem *item = new IntervalItem(intervalList, i->first, i->second);
            intervalList->addTopLevelItem(item);
        }
        if (calDay->state() == CalendarDay::Working) {
            //kDebug()<<k_funcinfo<<"("<<date.toString()<<") is workday"<<endl;
            state->setCurrentIndex(2);
            slotStateActivated(2);
            bApply->setEnabled(calDay->workingIntervals().count() > 0);
        } else if (calDay->state() == CalendarDay::NonWorking){
            //kDebug()<<k_funcinfo<<"("<<date.toString()<<") is holiday"<<endl;
            state->setCurrentIndex(1);
            slotStateActivated(1);
            bApply->setEnabled(true);
        } else  {
            //kDebug()<<k_funcinfo<<"("<<date.toString()<<")=none"<<endl;
            state->setCurrentIndex(0);
            slotStateActivated(0);
            bApply->setEnabled(true);
        }
    } else {
        // default
        state->setCurrentIndex(0);
        slotStateActivated(0);
        bApply->setEnabled(true);
    }
}

void CalendarEdit::slotWeekdaySelected(int day_/* 1..7 */) {
    if (m_calendar == 0 || day_ < 1 || day_ > 7) {
        kError()<<k_funcinfo<<"No calendar or weekday ("<<day_<<") not defined!"<<endl;
        return;
    }
    //kDebug()<<k_funcinfo<<"("<<day_<<")"<<endl;
    clearEditPart();
    CalendarDay *calDay = m_calendar->weekday(day_);
    if (!calDay) {
        kError()<<k_funcinfo<<"Weekday ("<<day_<<") not defined!"<<endl;
        return;
    }
    state->clear();
    state->addItem(i18n("Undefined"));
    state->addItem(i18n("Non-working"));
    state->addItem(i18n("Working"));
    foreach (TimeInterval *i, calDay->workingIntervals()) {
        IntervalItem *item = new IntervalItem(intervalList, i->first, i->second);
        intervalList->addTopLevelItem(item);
    }
    state->setEnabled(true);
    if (calDay->state() == CalendarDay::Working) {
        //kDebug()<<k_funcinfo<<"("<<day_<<")=workday"<<endl;
        state->setCurrentIndex(2);
        slotStateActivated(2);
        bApply->setEnabled(calDay->workingIntervals().count() > 0);
    } else if (calDay->state() == CalendarDay::NonWorking) {
        //kDebug()<<k_funcinfo<<"("<<day_<<")=Holiday"<<endl;
        state->setCurrentIndex(1);
        slotStateActivated(1);
        bApply->setEnabled(true);
    } else {
        //kDebug()<<k_funcinfo<<"("<<day_<<")=none"<<endl;
        state->setCurrentIndex(0);
        slotStateActivated(0);
        bApply->setEnabled(true);
    }
    slotCheckAllFieldsFilled();
}

void CalendarEdit::slotSelectionCleared() {
    clearEditPart();
    slotCheckAllFieldsFilled();
}

//----------------------------------------------------
CalendarEditDialog::CalendarEditDialog(Project &p, Calendar *cal, QWidget *parent, const char *name)
    : KDialog( parent),
      project(p),
      original( cal ),
      calendar( new Calendar() )
{
    *calendar = *cal;
    
    setCaption( i18n("Calendar's Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<k_funcinfo<<&p<<endl;
    dia = new CalendarEdit( this );
    dia->setCalendar( calendar, cal->timeZone()->name(), (bool)cal->parentCal() );
    setMainWidget(dia);
    enableButtonOk(false);

    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(dia, SIGNAL(obligatedFieldsFilled(bool)), SLOT(enableButtonOk(bool)));
}

CalendarEditDialog::~CalendarEditDialog()
{
    delete calendar;
}

K3Command *CalendarEditDialog::buildCommand(Part *part) {
    //kDebug()<<k_funcinfo<<endl;
    K3MacroCommand *macro=0;
    if (original->name() != calendar->name()) {
        if (macro == 0) macro = new K3MacroCommand("");
        macro->addCommand(new CalendarModifyNameCmd(part, original, calendar->name()));
    }
    //kDebug()<<k_funcinfo<<"Check for days deleted: "<<calendar->name()<<endl;
    foreach (CalendarDay *day, original->days()) {
        if (calendar->findDay(day->date()) == 0) {
            if (macro == 0) macro = new K3MacroCommand("");
            macro->addCommand(new CalendarRemoveDayCmd(part, original, day->date()));
            //kDebug()<<k_funcinfo<<"Removed day "<<day->date()<<endl;
        }
    }
    //kDebug()<<k_funcinfo<<"Check for days added or modified: "<<calendar->name()<<endl;
    foreach (CalendarDay *c, calendar->days()) {
        CalendarDay *day = original->findDay(c->date());
        if (day == 0) {
            if (macro == 0) macro = new K3MacroCommand("");
            // added
            //kDebug()<<k_funcinfo<<"Added day "<<c->date()<<endl;
            macro->addCommand(new CalendarAddDayCmd(part, original, new CalendarDay(c)));
        } else if (*day != c) {
            if (macro == 0) macro = new K3MacroCommand("");
            // modified
            //kDebug()<<k_funcinfo<<"Modified day "<<c->date()<<endl;
            macro->addCommand(new CalendarModifyDayCmd(part, original, new CalendarDay(c)));
        }
    }
    //kDebug()<<k_funcinfo<<"Check for weekdays modified: "<<calendar->name()<<endl;
    CalendarDay *day = 0, *org = 0;
    for (int i=1; i <= 7; ++i) {
        day = calendar->weekdays()->weekday(i);
        org = original->weekdays()->weekday(i);
        if (day && org) {
            if (*org != *day) {
                if (macro == 0) macro = new K3MacroCommand("");
                //kDebug()<<k_funcinfo<<"Weekday["<<i<<"] modified"<<endl;
                macro->addCommand(new CalendarModifyWeekdayCmd(part, original, i, new CalendarDay(day)));
            }
        } else if (day) {
            // shouldn't happen: hmmm, add day to original??
            kError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
        } else if (org) {
            // shouldn't happen: set org to default??
            kError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
        }
    }
    // timezone
    if ( original->timeZone() != calendar->timeZone() ) {
        if (macro == 0) macro = new K3MacroCommand("");
        macro->addCommand( new CalendarModifyTimeZoneCmd( part, original, calendar->timeZone() ) );
    }
    if (macro) {
        macro->setName(i18n("Modify Calendar"));
    }
    return macro;
}

void CalendarEditDialog::slotOk()
{
}

}  //KPlato namespace

#include "kptcalendaredit.moc"
