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

#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <q3datetimeedit.h>
#include <qdatetime.h>

#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

namespace KPlato
{

StandardWorktimeDialog::StandardWorktimeDialog(Project &p, QWidget *parent, const char *name)
    : KDialog(parent),
      project(p)
{
    setCaption( i18n("Standard Worktime") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonSeparator( true );
    //kDebug()<<k_funcinfo<<&p<<endl;
    m_original = p.standardWorktime();
    dia = new StandardWorktimeDialogImpl(m_original, this);

    setMainWidget(dia);
    enableButtonOk(false);

    connect(dia, SIGNAL(obligatedFieldsFilled(bool) ), SLOT(enableButtonOk(bool)));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOk(bool)));
}

KMacroCommand *StandardWorktimeDialog::buildCommand(Part *part) {
    kDebug()<<k_funcinfo<<endl;
    QString n = i18n("Modify Standard Worktime");
    KMacroCommand *cmd = 0;
    if (m_original->year() != dia->inYear()) {
        if (cmd == 0) cmd = new KMacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeYearCmd(part, m_original, m_original->year(), dia->inYear()));
    }
    if (m_original->month() != dia->inMonth()) {
        if (cmd == 0) cmd = new KMacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeMonthCmd(part, m_original, m_original->month(), dia->inMonth()));
    }
    if (m_original->week() != dia->inWeek()) {
        if (cmd == 0) cmd = new KMacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeWeekCmd(part, m_original, m_original->week(), dia->inWeek()));
    }
    if (m_original->day() != dia->inDay()) {
        if (cmd == 0) cmd = new KMacroCommand(n);
        cmd->addCommand(new ModifyStandardWorktimeDayCmd(part, m_original, m_original->day(), dia->inDay()));
    }
    return cmd;

}

void StandardWorktimeDialog::slotOk() {
    accept();
}


StandardWorktimeDialogImpl::StandardWorktimeDialogImpl(StandardWorktime *std, QWidget *parent)
    : StandardWorktimeDialogBase(parent),
      m_std(std) {
    if (!std) {
        m_std = new StandardWorktime();
    }
    m_year = m_std->year();
    m_month = m_std->month();
    m_week = m_std->week();
    m_day = m_std->day();

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
    //kDebug()<<k_funcinfo<<value<<endl;
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
