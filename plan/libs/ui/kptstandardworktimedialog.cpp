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

#include "kptstandardworktimedialog.h"
#include "kptduration.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcommand.h"

#include <QPushButton>
#include <QTreeWidgetItem>

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

namespace KPlato
{

class WeekdayListItem : public QTreeWidgetItem
{
public:
    WeekdayListItem(Calendar *cal, int wd, QTreeWidget *parent, const QString& name, QTreeWidgetItem *after)
    : QTreeWidgetItem(parent, after),
      original(cal->weekday(wd)),
      calendar(cal),
      weekday(wd)
    {
        setText(0, name);
        day = new CalendarDay(original);
        if (day->state() == CalendarDay::NonWorking) {
            setHours();
        } else {
            setText(1, KGlobal::locale()->formatNumber(day->duration().toDouble(Duration::Unit_h)));
        }
    }
    ~WeekdayListItem() {
        delete day;
    }
    void setHours() {
        setText(1, "-");
        day->clearIntervals();
    }
    void setIntervals(QList<TimeInterval*> intervals) {
        day->setIntervals(intervals);
        setText(1, KGlobal::locale()->formatNumber(day->duration().toDouble(Duration::Unit_h)));
    }
    void setState(int st) {
        day->setState(st+1);
    }
    
    MacroCommand *save() {
        MacroCommand *cmd=0;
        if (*original != *day) {
            cmd = new MacroCommand();
            cmd->addCommand( new CalendarModifyWeekdayCmd(calendar, weekday, day) );
            day = 0;
        }
        return cmd;
    }
    CalendarDay *day;
    CalendarDay *original;
    Calendar *calendar;
    int weekday;
};

StandardWorktimeDialog::StandardWorktimeDialog(Project &p, QWidget *parent)
    : KDialog(parent),
      project(p)
{
    setCaption( i18n("Estimate Conversions") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<&p;
    m_original = p.standardWorktime();
    dia = new StandardWorktimeDialogImpl(m_original, this);

    setMainWidget(dia);
    enableButtonOk(false);

    connect(dia, SIGNAL(obligatedFieldsFilled(bool) ), SLOT(enableButtonOk(bool)));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOk(bool)));
    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
}

MacroCommand *StandardWorktimeDialog::buildCommand() {
    //kDebug();
    QString n = i18n("Modify Estimate Conversions");
    MacroCommand *cmd = 0;
    if (m_original->year() != dia->inYear()) {
        if (cmd == 0) cmd = new MacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeYearCmd(m_original, m_original->year(), dia->inYear()));
    }
    if (m_original->month() != dia->inMonth()) {
        if (cmd == 0) cmd = new MacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeMonthCmd(m_original, m_original->month(), dia->inMonth()));
    }
    if (m_original->week() != dia->inWeek()) {
        if (cmd == 0) cmd = new MacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeWeekCmd(m_original, m_original->week(), dia->inWeek()));
    }
    if (m_original->day() != dia->inDay()) {
        if (cmd == 0) cmd = new MacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeDayCmd(m_original, m_original->day(), dia->inDay()));
    }
    return cmd;

}

void StandardWorktimeDialog::slotOk() {
    accept();
}


StandardWorktimeDialogImpl::StandardWorktimeDialogImpl(StandardWorktime *std, QWidget *parent)
    : QWidget(parent),
      m_std(std) {
    
    setupUi(this);
    if (!std) {
        m_std = new StandardWorktime();
    }
    m_year = m_std->year();
    m_month = m_std->month();
    m_week = m_std->week();
    m_day = m_std->day();

    kDebug()<<"y="<<m_year<<" m="<<m_month<<" w="<<m_week<<" d="<<m_day;
    year->setValue(m_year);
    month->setValue(m_month);
    week->setValue(m_week);
    day->setValue(m_day);
    
    connect(year, SIGNAL(valueChanged(double)), SLOT(slotYearChanged(double)));
    connect(month, SIGNAL(valueChanged(double)), SLOT(slotMonthChanged(double)));
    connect(week, SIGNAL(valueChanged(double)), SLOT(slotWeekChanged(double)));
    connect(day, SIGNAL(valueChanged(double)), SLOT(slotDayChanged(double)));

}


void StandardWorktimeDialogImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void StandardWorktimeDialogImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled(true);
}

void StandardWorktimeDialogImpl::slotYearChanged(double value) {
    //kDebug()<<value;
    m_year = value;
    if (month->value() > value)
        month->setValue(value);
    slotEnableButtonOk(true);
}

void StandardWorktimeDialogImpl::slotMonthChanged(double value) {
    m_month = value;
    if (year->value() < value)
        year->setValue(value);
    if (week->value() > value)
        week->setValue(value);
    slotEnableButtonOk(true);
}

void StandardWorktimeDialogImpl::slotWeekChanged(double value) {
    m_week = value;
    if (month->value() < value)
        month->setValue(value);
    if (day->value() > value)
        day->setValue(value);
    slotEnableButtonOk(true);
}

void StandardWorktimeDialogImpl::slotDayChanged(double value) {
    m_day = value;
    if (week->value() < value)
        week->setValue(value);
    slotEnableButtonOk(true);
}


}  //KPlato namespace


#include "kptstandardworktimedialog.moc"
