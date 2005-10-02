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

#include "kptstandardworktimedialog.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcommand.h"
#include "kptintervaledit.h"
#include "kptpart.h"

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

KPTStandardWorktimeDialog::KPTStandardWorktimeDialog(KPTProject &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Standard Worktime"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    //kdDebug()<<k_funcinfo<<&p<<endl;
    m_original = p.standardWorktime();
    dia = new KPTStandardWorktimeDialogImpl(new KPTStandardWorktime(m_original), this); //FIXME

    setMainWidget(dia);
    enableButtonOK(false);

    connect(dia, SIGNAL(obligatedFieldsFilled(bool) ), SLOT(enableButtonOK(bool)));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOK(bool)));
}

// FIXME
KMacroCommand *KPTStandardWorktimeDialog::buildCommand(KPTPart *part) {
    kdDebug()<<k_funcinfo<<endl;
    project.addStandardWorktime(dia->standardWorktime());
    return 0;
    
}

void KPTStandardWorktimeDialog::slotOk() {
    accept();
}


KPTStandardWorktimeDialogImpl::KPTStandardWorktimeDialogImpl(KPTStandardWorktime *std, QWidget *parent) 
    : StandardWorktimeDialogBase(parent),
      m_std(std) {
    if (!std) {
        m_std = new KPTStandardWorktime();
    }
    year->setValue(m_std->durationYear().hours());
    month->setValue(m_std->durationMonth().hours());
    
    QTime t;
    
    dayTime->setTime(t.addSecs(m_std->durationDay().seconds()));
    
    int state = (m_std->state(0) == KPTMap::Working) ? 0 : 1;
    stateMonday->setCurrentItem(state);
    slotStateMondayActivated(state);
    mondayTime->setTime(t.addSecs(m_std->durationWeekday(0).seconds()));
    
    state = (m_std->state(1) == KPTMap::Working) ? 0 : 1;
    stateTuesday->setCurrentItem(state);
    slotStateTuesdayActivated(state);
    tuesdayTime->setTime(t.addSecs(m_std->durationWeekday(1).seconds()));
    
    state = (m_std->state(2) == KPTMap::Working) ? 0 : 1;
    stateWednesday->setCurrentItem(state);
    slotStateWednesdayActivated(state);
    wednesdayTime->setTime(t.addSecs(m_std->durationWeekday(2).seconds()));

    state = (m_std->state(3) == KPTMap::Working) ? 0 : 1;
    stateThursday->setCurrentItem(state);
    slotStateThursdayActivated(state);
    thursdayTime->setTime(t.addSecs(m_std->durationWeekday(3).seconds()));
    
    state = (m_std->state(4) == KPTMap::Working) ? 0 : 1;
    stateFriday->setCurrentItem(state);
    slotStateFridayActivated(state);
    fridayTime->setTime(t.addSecs(m_std->durationWeekday(4).seconds()));
    
    state = (m_std->state(5) == KPTMap::Working) ? 0 : 1;
    stateSaturday->setCurrentItem(state);
    slotStateSaturdayActivated(state);
    saturdayTime->setTime(t.addSecs(m_std->durationWeekday(5).seconds()));
    
    state = (m_std->state(6) == KPTMap::Working) ? 0 : 1;
    stateSunday->setCurrentItem(state);
    slotStateSundayActivated(state);
    sundayTime->setTime(t.addSecs(m_std->durationWeekday(6).seconds()));
    
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));

    connect(year, SIGNAL(valueChanged(int)), SLOT(slotYearChanged(int)));
    connect(month, SIGNAL(valueChanged(int)), SLOT(slotMonthChanged(int)));
    
    connect(bEditDay, SIGNAL(clicked()), SLOT(slotEditDayClicked()));
    
    connect(bEditMonday, SIGNAL(clicked()), SLOT(slotEditMondayClicked()));
    connect(bEditTuesday, SIGNAL(clicked()), SLOT(slotEditTuesdayClicked()));
    connect(bEditWednesday, SIGNAL(clicked()), SLOT(slotEditWednesdayClicked()));
    connect(bEditThursday, SIGNAL(clicked()), SLOT(slotEditThursdayClicked()));
    connect(bEditFriday, SIGNAL(clicked()), SLOT(slotEditFridayClicked()));
    connect(bEditSaturday, SIGNAL(clicked()), SLOT(slotEditSaturdayClicked()));
    connect(bEditSunday, SIGNAL(clicked()), SLOT(slotEditSundayClicked()));
    
    connect(stateMonday, SIGNAL(activated(int)), SLOT(slotStateMondayActivated(int)));
    connect(stateTuesday, SIGNAL(activated(int)), SLOT(slotStateTuesdayActivated(int)));
    connect(stateWednesday, SIGNAL(activated(int)), SLOT(slotStateWednesdayActivated(int)));
    connect(stateThursday, SIGNAL(activated(int)), SLOT(slotStateThursdayActivated(int)));
    connect(stateFriday, SIGNAL(activated(int)), SLOT(slotStateFridayActivated(int)));
    connect(stateSaturday, SIGNAL(activated(int)), SLOT(slotStateSaturdayActivated(int)));
    connect(stateSunday, SIGNAL(activated(int)), SLOT(slotStateSundayActivated(int)));

    connect(mondayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotMondayTimeChanged(const QTime&)));
    connect(tuesdayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotTuesdayTimeChanged(const QTime&)));
    connect(wednesdayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotWednesdayTimeChanged(const QTime&)));
    connect(thursdayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotThursdayTimeChanged(const QTime&)));
    connect(fridayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotFridayTimeChanged(const QTime&)));
    connect(saturdayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotSaturdayTimeChanged(const QTime&)));
    connect(sundayTime, SIGNAL(valueChanged(const QTime&)), SLOT(slotSundayTimeTimeChanged(const QTime&)));
}


void KPTStandardWorktimeDialogImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void KPTStandardWorktimeDialogImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled(true);
}

void KPTStandardWorktimeDialogImpl::slotYearChanged(int value) {
    kdDebug()<<k_funcinfo<<value<<endl;
    m_std->setYear(KPTDuration(0, value, 0));
    slotEnableButtonOk(true);
}

void KPTStandardWorktimeDialogImpl::slotMonthChanged(int value) {
    m_std->setMonth(KPTDuration(0, value, 0));
    slotEnableButtonOk(true);
}

void KPTStandardWorktimeDialogImpl::slotEditDayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(), this);
    if (dia->exec()) {
        m_std->clearIntervals();
        m_std->setIntervals(dia->intervals());
        dayTime->setTime(QTime().addSecs(m_std->durationDay().seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditMondayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(0), this);
    if (dia->exec()) {
        m_std->clearIntervals(0);
        m_std->setIntervals(0, dia->intervals());
        mondayTime->setTime(QTime().addSecs(m_std->durationWeekday(0).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditTuesdayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(1), this);
    if (dia->exec()) {
        m_std->clearIntervals(1);
        m_std->setIntervals(1, dia->intervals());
        tuesdayTime->setTime(QTime().addSecs(m_std->durationWeekday(1).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditWednesdayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(2), this);
    if (dia->exec()) {
        m_std->clearIntervals(2);
        m_std->setIntervals(2, dia->intervals());
        wednesdayTime->setTime(QTime().addSecs(m_std->durationWeekday(2).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditThursdayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(3), this);
    if (dia->exec()) {
        m_std->clearIntervals(3);
        m_std->setIntervals(3, dia->intervals());
        thursdayTime->setTime(QTime().addSecs(m_std->durationWeekday(3).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditFridayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(4), this);
    if (dia->exec()) {
        m_std->clearIntervals(4);
        m_std->setIntervals(4, dia->intervals());
        fridayTime->setTime(QTime().addSecs(m_std->durationWeekday(4).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditSaturdayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(5), this);
    if (dia->exec()) {
        m_std->clearIntervals(5);
        m_std->setIntervals(5, dia->intervals());
        saturdayTime->setTime(QTime().addSecs(m_std->durationWeekday(5).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotEditSundayClicked() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTIntervalEdit *dia = new KPTIntervalEdit(m_std->intervals(6), this);
    if (dia->exec()) {
        m_std->clearIntervals(6);
        m_std->setIntervals(6, dia->intervals());
        sundayTime->setTime(QTime().addSecs(m_std->durationWeekday(6).seconds()));
        slotEnableButtonOk(true);
    }
}

void KPTStandardWorktimeDialogImpl::slotStateMondayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditMonday->setEnabled(true);
        m_std->setState(0, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(0);
        bEditMonday->setEnabled(false);
        m_std->setState(0, KPTMap::NonWorking);
        mondayTime->setTime(QTime().addSecs(m_std->durationWeekday(0).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateTuesdayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditTuesday->setEnabled(true);
        m_std->setState(1, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(1);
        bEditTuesday->setEnabled(false);
        m_std->setState(1, KPTMap::NonWorking);
        tuesdayTime->setTime(QTime().addSecs(m_std->durationWeekday(1).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateWednesdayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditWednesday->setEnabled(true);
        m_std->setState(2, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(2);
        bEditWednesday->setEnabled(false);
        m_std->setState(2, KPTMap::NonWorking);
        wednesdayTime->setTime(QTime().addSecs(m_std->durationWeekday(2).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateThursdayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditThursday->setEnabled(true);
        m_std->setState(3, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(3);
        bEditThursday->setEnabled(false);
        m_std->setState(3, KPTMap::NonWorking);
        thursdayTime->setTime(QTime().addSecs(m_std->durationWeekday(3).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateFridayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditFriday->setEnabled(true);
        m_std->setState(4, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(4);
        bEditFriday->setEnabled(false);
        m_std->setState(4, KPTMap::NonWorking);
        fridayTime->setTime(QTime().addSecs(m_std->durationWeekday(4).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateSaturdayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditSaturday->setEnabled(true);
        m_std->setState(5, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(5);
        bEditSaturday->setEnabled(false);
        m_std->setState(5, KPTMap::NonWorking);
        saturdayTime->setTime(QTime().addSecs(m_std->durationWeekday(5).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotStateSundayActivated(int index) {
    //kdDebug()<<k_funcinfo<<": "<<index<<endl;
    if (index == 0) {
        bEditSunday->setEnabled(true);
        m_std->setState(6, KPTMap::Working);
    } else if (index == 1) {
        m_std->clearIntervals(6);
        bEditSunday->setEnabled(false);
        m_std->setState(6, KPTMap::NonWorking);
        sundayTime->setTime(QTime().addSecs(m_std->durationWeekday(6).seconds()));
    }
}

void KPTStandardWorktimeDialogImpl::slotMondayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotTuesdayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotWednesdayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotThursdayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotFridayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotSaturdayTimeChanged(const QTime& time) {
    kdDebug()<<k_funcinfo<<endl;
    weekTime->setText(QString("%1").arg(m_std->durationWeek().toString(KPTDuration::Format_HourFraction)));
}

void KPTStandardWorktimeDialogImpl::slotSundayTimeTimeChanged(const QTime& time) {
}

}  //KPlato namespace

#include "kptstandardworktimedialog.moc"
