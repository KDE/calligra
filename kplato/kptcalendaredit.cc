/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kptcalendaredit.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcalendarpanel.h"
#include "kptmap.h"
#include "intervalitem.h"

#include <q3buttongroup.h>
#include <q3header.h>
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

#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QMap>

#include <kdebug.h>

namespace KPlato
{

CalendarEdit::CalendarEdit (QWidget *parent, const char */*name*/)
    : CalendarEditBase(parent),
      m_calendar(0)
 {

    clear();
    intervalList->header()->setStretchEnabled(true, 0);
    intervalList->setShowSortIndicator(true);
    intervalList->setSorting(0);

    connect (calendarPanel, SIGNAL(dateChanged(QDate)), SLOT(slotDateSelected(QDate)));
    connect (calendarPanel, SIGNAL(weekdaySelected(int)), SLOT(slotWeekdaySelected(int)));
    connect(calendarPanel, SIGNAL(selectionCleared()), SLOT(slotSelectionCleared()));

    connect (state, SIGNAL(activated(int)), SLOT(slotStateActivated(int)));
    connect (bClear, SIGNAL(clicked()), SLOT(slotClearClicked()));
    connect (bAddInterval, SIGNAL(clicked()), SLOT(slotAddIntervalClicked()));

    connect (bApply, SIGNAL(clicked()), SLOT(slotApplyClicked()));
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
        bApply->setEnabled(intervalList->firstChild());
    }
}

void CalendarEdit::slotClearClicked() {
    //kDebug()<<k_funcinfo<<endl;
    intervalList->clear();
    bApply->setEnabled(false);
}
void CalendarEdit::slotAddIntervalClicked() {
    //kDebug()<<k_funcinfo<<endl;
    intervalList->insertItem(new IntervalItem(intervalList, startTime->time(), endTime->time()));
    bApply->setEnabled(true);
}

//NOTE: enum Map::State must match combobox state!
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
        if (calDay->state() == Map::Working) {
            for (Q3ListViewItem *item = intervalList->firstChild(); item; item = item->nextSibling()) {
                //kDebug()<<k_funcinfo<<"Adding interval: "<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString()<<endl;
                calDay->addInterval(static_cast<IntervalItem *>(item)->interval());
            }
        }
    }

    IntMap weekdays = calendarPanel->selectedWeekdays();
    for(IntMap::iterator it = weekdays.begin(); it != weekdays.end(); ++it) {
        //kDebug()<<k_funcinfo<<"weekday="<<it.key()<<endl;
        CalendarDay *weekday = m_calendar->weekday(it.key()-1);
        weekday->setState(state->currentIndex());//NOTE!!
        weekday->clearIntervals();
        if (weekday->state() == Map::Working) {
            for (Q3ListViewItem *item = intervalList->firstChild(); item; item = item->nextSibling()) {
                //kDebug()<<k_funcinfo<<"Adding interval: "<<static_cast<IntervalItem *>(item)->interval().first.toString()<<"-"<<static_cast<IntervalItem *>(item)->interval().second.toString()<<endl;
                weekday->addInterval(static_cast<IntervalItem *>(item)->interval());
            }
        }
    }

    calendarPanel->markSelected(state->currentIndex()); //NOTE!!
    emit applyClicked();
    slotCheckAllFieldsFilled();
}

void CalendarEdit::slotCheckAllFieldsFilled() {
    //kDebug()<<k_funcinfo<<endl;
    if (state->currentItem() == 0 /*undefined*/ ||
        state->currentIndex() == 1 /*Non-working*/||
        (state->currentItem() == 2 /*Working*/ && intervalList->firstChild()))
    {
        emit obligatedFieldsFilled(true);
    }
    else if (state->currentIndex() == 2 && !intervalList->firstChild())
    {
        emit obligatedFieldsFilled(false);
    }
}

void CalendarEdit::setCalendar(Calendar *cal) {
    m_calendar = cal;
    clear();
    calendarPanel->setCalendar(cal);
}

void CalendarEdit::clear() {
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

void CalendarEdit::slotDateSelected(QDate date) {
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
        Q3PtrListIterator<QPair<QTime, QTime> > it = calDay->workingIntervals();
        for (; it.current(); ++it) {
            IntervalItem *item = new IntervalItem(intervalList, it.current()->first, it.current()->second);
            intervalList->insertItem(item);
        }
        if (calDay->state() == Map::Working) {
            //kDebug()<<k_funcinfo<<"("<<date.toString()<<") is workday"<<endl;
            state->setCurrentIndex(2);
            slotStateActivated(2);
            bApply->setEnabled(calDay->workingIntervals().count() > 0);
        } else if (calDay->state() == Map::NonWorking){
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
    CalendarDay *calDay = m_calendar->weekday(day_-1); // 0..6
    if (!calDay) {
        kError()<<k_funcinfo<<"Weekday ("<<day_<<") not defined!"<<endl;
        return;
    }
    state->clear();
    state->addItem(i18n("Undefined"));
    state->addItem(i18n("Non-working"));
    state->addItem(i18n("Working"));
    Q3PtrListIterator<QPair<QTime, QTime> > it = calDay->workingIntervals();
    for (; it.current(); ++it) {
        IntervalItem *item = new IntervalItem(intervalList, it.current()->first, it.current()->second);
        intervalList->insertItem(item);
    }
    state->setEnabled(true);
    if (calDay->state() == Map::Working) {
        //kDebug()<<k_funcinfo<<"("<<day_<<")=workday"<<endl;
        state->setCurrentIndex(2);
        slotStateActivated(2);
        bApply->setEnabled(calDay->workingIntervals().count() > 0);
    } else if (calDay->state() == Map::NonWorking) {
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
}

void CalendarEdit::slotSelectionCleared() {
    clearEditPart();
}

}  //KPlato namespace

#include "kptcalendaredit.moc"
