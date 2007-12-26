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
#include <QPushButton>
#include <qradiobutton.h>
#include <q3textedit.h>
#include <q3datetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <q3textbrowser.h>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <k3command.h>
#include <klocale.h>
#include <kdebug.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>

namespace KPlato
{

CalendarEdit::CalendarEdit (QWidget *parent)
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
    connect (tz, SIGNAL( activated( int ) ), SLOT( slotTimeZoneChanged( int ) ) );
}

void CalendarEdit::slotTimeZoneChanged( int )
{
    //kDebug();
    QStringList lst;
    QString name = tz->currentText();
    KTimeZone tz;
    foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
        if ( name == i18n( s.toUtf8() ) ) {
            if ( s != m_calendar->timeZone().name() ) {
                tz = KSystemTimeZones::zone( s );
            }
            break;
        }
    }
    if ( tz.isValid() ) {
        m_calendar->setTimeZone( tz );
        m_changed = true;
        emit timeZoneChanged();
        slotCheckAllFieldsFilled();
    }
}

void CalendarEdit::slotStateActivated(int id) {
    //kDebug()<<"id="<<id;
    if (id == 0) { // undefined
        startTime->setEnabled(false);
        length->setEnabled(false);
        bClear->setEnabled(false);
        bAddInterval->setEnabled(false);
        intervalList->setEnabled(false);
        bApply->setEnabled(true);
    } else if (id == 1) { // non working
        startTime->setEnabled(false);
        length->setEnabled(false);
        bClear->setEnabled(false);
        bAddInterval->setEnabled(false);
        intervalList->setEnabled(false);
        bApply->setEnabled(true);
    } else if (id == 2) { //working
        startTime->setEnabled(true);
        length->setEnabled(true);
        bClear->setEnabled(true);
        bAddInterval->setEnabled(true);
        intervalList->setEnabled(true);
        bApply->setEnabled(intervalList->topLevelItemCount() > 0);
    }
}

void CalendarEdit::slotClearClicked() {
    //kDebug();
    intervalList->clear();
    bApply->setEnabled(false);
}
void CalendarEdit::slotAddIntervalClicked() {
    //kDebug();
    intervalList->addTopLevelItem( new IntervalItem(intervalList, startTime->time(), length->value() ) );
    bApply->setEnabled(true);
}

//NOTE: enum CalendarDay::State must match combobox state!
void CalendarEdit::slotApplyClicked() {
    //kDebug()<<"("<<m_calendar<<")";
    DateMap dates = calendarPanel->selectedDates();
    for(DateMap::iterator it = dates.begin(); it != dates.end(); ++it) {
        QDate date = QDate::fromString(it.key(), Qt::ISODate);
        //kDebug()<<"Date:"<<date;
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
                //kDebug()<<"Adding interval:"<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString();
                calDay->addInterval(static_cast<IntervalItem *>(intervalList->topLevelItem(i))->interval());
            }
        }
    }

    IntMap weekdays = calendarPanel->selectedWeekdays();
    for(IntMap::iterator it = weekdays.begin(); it != weekdays.end(); ++it) {
        //kDebug()<<"weekday="<<it.key();
        CalendarDay *weekday = m_calendar->weekday(it.key());
        weekday->setState(state->currentIndex());//NOTE!!
        weekday->clearIntervals();
        if (weekday->state() == CalendarDay::Working) {
            int cnt = intervalList->topLevelItemCount();
            for (int i = 0; i < cnt; ++i) {
                //kDebug()<<"Adding interval:"<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString();
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
    //kDebug();
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
    else if (state->currentIndex() == 2 && !(intervalList->topLevelItemCount() > 0))
    {
        emit obligatedFieldsFilled(false);
    }
}

void CalendarEdit::setCalendar(Calendar *cal, const QString &tzString, bool disable)
{
    m_calendar = cal;
    clear();
    if ( disable ) {
        tz->addItem( i18n( tzString.toUtf8() ) );
    } else {
        QStringList lst;
        foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
            lst << i18n( s.toUtf8() );
        }
        lst.sort();
        tz->addItems( lst );
        tz->setCurrentIndex( lst.indexOf( i18n( tzString.toUtf8() ) ) );
    }
    calendarPanel->setCalendar(cal);
}

void CalendarEdit::clear() {
    tz->clear();
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
    length->setEnabled(false);
    length->setValue( 8.0 ); //FIXME

    bAddInterval->setEnabled(false);
    bClear->setEnabled(false);
    bApply->setEnabled(false);
    state->setEnabled(false);
}

void CalendarEdit::slotDateSelected(const QDate& date) {
    if (m_calendar == 0)
        return;
    //kDebug()<<"("<<date.toString()<<")";
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
            //kDebug()<<"("<<date.toString()<<") is workday";
            state->setCurrentIndex(2);
            slotStateActivated(2);
            bApply->setEnabled(calDay->workingIntervals().count() > 0);
        } else if (calDay->state() == CalendarDay::NonWorking){
            //kDebug()<<"("<<date.toString()<<") is holiday";
            state->setCurrentIndex(1);
            slotStateActivated(1);
            bApply->setEnabled(true);
        } else  {
            //kDebug()<<"("<<date.toString()<<")=none";
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
        kError()<<"No calendar or weekday ("<<day_<<") not defined!"<<endl;
        return;
    }
    //kDebug()<<"("<<day_<<")";
    clearEditPart();
    CalendarDay *calDay = m_calendar->weekday(day_);
    if (!calDay) {
        kError()<<"Weekday ("<<day_<<") not defined!"<<endl;
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
        //kDebug()<<"("<<day_<<")=workday";
        state->setCurrentIndex(2);
        slotStateActivated(2);
        bApply->setEnabled(calDay->workingIntervals().count() > 0);
    } else if (calDay->state() == CalendarDay::NonWorking) {
        //kDebug()<<"("<<day_<<")=Holiday";
        state->setCurrentIndex(1);
        slotStateActivated(1);
        bApply->setEnabled(true);
    } else {
        //kDebug()<<"("<<day_<<")=none";
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
CalendarEditDialog::CalendarEditDialog(Project &p, Calendar *cal, QWidget *parent)
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
    //kDebug()<<&p;
    dia = new CalendarEdit( this );
    dia->setCalendar( calendar, cal->timeZone().name(), (bool)cal->parentCal() );
    setMainWidget(dia);
    enableButtonOk(false);

    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(dia, SIGNAL(obligatedFieldsFilled(bool)), SLOT(enableButtonOk(bool)));
}

CalendarEditDialog::~CalendarEditDialog()
{
    delete calendar;
}

MacroCommand *CalendarEditDialog::buildCommand() {
    //kDebug();
    MacroCommand *macro=0;
    if (original->name() != calendar->name()) {
        if (macro == 0) macro = new MacroCommand("");
        macro->addCommand(new CalendarModifyNameCmd(original, calendar->name()));
    }
    //kDebug()<<"Check for days deleted:"<<calendar->name();
    foreach (CalendarDay *day, original->days()) {
        if (calendar->findDay(day->date()) == 0) {
            if (macro == 0) macro = new MacroCommand("");
            macro->addCommand(new CalendarRemoveDayCmd(original, day->date()));
            //kDebug()<<"Removed day"<<day->date();
        }
    }
    //kDebug()<<"Check for days added or modified:"<<calendar->name();
    foreach (CalendarDay *c, calendar->days()) {
        CalendarDay *day = original->findDay(c->date());
        if (day == 0) {
            if (macro == 0) macro = new MacroCommand("");
            // added
            //kDebug()<<"Added day"<<c->date();
            macro->addCommand(new CalendarAddDayCmd(original, new CalendarDay(c)));
        } else if (*day != c) {
            if (macro == 0) macro = new MacroCommand("");
            // modified
            //kDebug()<<"Modified day"<<c->date();
            macro->addCommand(new CalendarModifyDayCmd(original, new CalendarDay(c)));
        }
    }
    //kDebug()<<"Check for weekdays modified:"<<calendar->name();
    CalendarDay *day = 0, *org = 0;
    for (int i=1; i <= 7; ++i) {
        day = calendar->weekdays()->weekday(i);
        org = original->weekdays()->weekday(i);
        if (day && org) {
            if (*org != *day) {
                if (macro == 0) macro = new MacroCommand("");
                //kDebug()<<"Weekday["<<i<<"] modified";
                macro->addCommand(new CalendarModifyWeekdayCmd(original, i, new CalendarDay(day)));
            }
        } else if (day) {
            // shouldn't happen: hmmm, add day to original??
            kError()<<"Should always have 7 weekdays"<<endl;
        } else if (org) {
            // shouldn't happen: set org to default??
            kError()<<"Should always have 7 weekdays"<<endl;
        }
    }
    // tz
    if ( original->timeZone() != calendar->timeZone() ) {
        if (macro == 0) macro = new MacroCommand("");
        macro->addCommand( new CalendarModifyTimeZoneCmd( original, calendar->timeZone() ) );
    }
    if (macro) {
        macro->setText(i18n("Modify Calendar"));
    }
    return macro;
}

void CalendarEditDialog::slotOk()
{
}

}  //KPlato namespace

#include "kptcalendaredit.moc"
