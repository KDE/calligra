/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskprogresspanel.h"

#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <QCheckBox>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatetimewidget.h>
#include <knuminput.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcommand.h>

#include <kdebug.h>

#include "kpttask.h"
#include "kptcommand.h"
#include "kptdurationwidget.h"
#include "kptcalendar.h"

namespace KPlato
{

TaskProgressPanel::TaskProgressPanel(Task &task, StandardWorktime *workTime, QWidget *parent, const char *name)
    : TaskProgressPanelImpl(parent, name),
      m_task(task),
      m_completion( task.completion() ),
      m_dayLength(24)
{
    kDebug()<<k_funcinfo<<endl;
    started->setChecked(m_completion.started());
    finished->setChecked(m_completion.finished());
    startTime->setDateTime(m_completion.startTime());
    finishTime->setDateTime(m_completion.finishTime());
    
    if ( m_completion.entryDate() < QDate::currentDate() ) {
        dateEdit->setDate( QDate::currentDate() );
    } else {
        dateEdit->setDate( m_completion.entryDate() );
    } 
    percentFinished->setValue(m_completion.percentFinished());
    
    if (workTime) {
        kDebug()<<k_funcinfo<<"daylength="<<workTime->durationDay().hours()<<endl;
        m_dayLength = workTime->durationDay().hours();
        setEstimateScales(m_dayLength);
    }
    remainingEffort->setValue(m_completion.remainingEffort());
    remainingEffort->setVisibleFields(DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes);
    remainingEffort->setFieldUnit(0, i18nc("day", "d"));
    remainingEffort->setFieldUnit(1, i18nc("hour", "h"));
    remainingEffort->setFieldUnit(2, i18nc("minute", "m"));

    m_completion.totalPerformed() = task.actualEffort(); //FIXME
    actualEffort->setValue(m_completion.totalPerformed());
    actualEffort->setVisibleFields(DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes);
    actualEffort->setFieldUnit(0, i18nc("day", "d"));
    actualEffort->setFieldUnit(1, i18nc("hour", "h"));
    actualEffort->setFieldUnit(2, i18nc("minute", "m"));
    
    scheduledStart->setDateTime(task.startTime());
    scheduledFinish->setDateTime(task.endTime());
    scheduledEffort->setValue(task.effort()->expected());
    scheduledEffort->setVisibleFields(DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes);
    scheduledEffort->setFieldUnit(0, i18nc("day", "d"));
    scheduledEffort->setFieldUnit(1, i18nc("hour", "h"));
    scheduledEffort->setFieldUnit(2, i18nc("minute", "m"));
    
    enableWidgets();
    started->setFocus();
    
}


bool TaskProgressPanel::ok() {
    return true;
}

KCommand *TaskProgressPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = 0;
    QString c = i18n("Modify task completion");
    
    if ( m_completion.started() != started->isChecked() ) {
        if ( cmd == 0 ) cmd = new KMacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartedCmd(part, m_completion, started->isChecked() ) );
    }
    if ( m_completion.finished() != finished->isChecked() ) {
        if ( cmd == 0 ) cmd = new KMacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishedCmd(part, m_completion, finished->isChecked() ) );
    }
    if ( m_completion.startTime() != startTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new KMacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartTimeCmd(part, m_completion, startTime->dateTime() ) );
    }
    if ( m_completion.finishTime() != finishTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new KMacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishTimeCmd(part, m_completion, finishTime->dateTime() ) );
    }
    if ( m_completion.entryDate() != dateEdit->date() ) {
        if ( cmd == 0 ) cmd = new KMacroCommand( c );
        Completion::Entry *e = new Completion::Entry( percentFinished->value(), remainingEffort->value(), actualEffort->value() );
        cmd->addCommand( new AddCompletionEntryCmd(part, m_completion, dateEdit->date(), e ) );
    } else {
        if ( ( m_completion.percentFinished() != percentFinished->value() ) ||
             ( m_completion.remainingEffort()  != remainingEffort->value() ) ||
             ( m_completion.totalPerformed() != actualEffort->value() ) ) {
            if ( cmd == 0 ) cmd = new KMacroCommand( c );
            Completion::Entry *e = new Completion::Entry( percentFinished->value(), remainingEffort->value(), actualEffort->value() );
            cmd->addCommand( new AddCompletionEntryCmd(part, m_completion, dateEdit->date(), e ) );
        }
    }
    return cmd;
}

void TaskProgressPanel::setEstimateScales( int day )
{
    remainingEffort->setFieldScale(0, day);
    remainingEffort->setFieldRightscale(0, day);
    remainingEffort->setFieldLeftscale(1, day);

    actualEffort->setFieldScale(0, day);
    actualEffort->setFieldRightscale(0, day);
    actualEffort->setFieldLeftscale(1, day);

    scheduledEffort->setFieldScale(0, day);
    scheduledEffort->setFieldRightscale(0, day);
    scheduledEffort->setFieldLeftscale(1, day);
}

//-------------------------------------

TaskProgressPanelImpl::TaskProgressPanelImpl(QWidget *parent, const char *name)
    : QWidget(parent) {
    
    setObjectName(name);
    setupUi(this);
    actualEffort = new DurationWidget(actualEffortHolder);
    if (actualEffortHolder->layout()) 
        actualEffortHolder->layout()->addWidget(actualEffort);
    remainingEffort = new DurationWidget(remainingEffortHolder);
    if (remainingEffortHolder->layout()) 
        remainingEffortHolder->layout()->addWidget(remainingEffort);
    scheduledEffort = new DurationWidget(scheduledEffortHolder);
    if (scheduledEffortHolder->layout()) 
        scheduledEffortHolder->layout()->addWidget(scheduledEffort);

    connect(started, SIGNAL(toggled(bool)), SLOT(slotStartedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));

    connect(percentFinished, SIGNAL(valueChanged(int)), SLOT(slotPercentFinishedChanged(int)));
    connect(percentFinished, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
    
    connect(startTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    connect(finishTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    
    connect(remainingEffort, SIGNAL(valueChanged()), SLOT(slotChanged()));
    connect(actualEffort, SIGNAL(valueChanged()), SLOT(slotChanged()));

}

void TaskProgressPanelImpl::slotChanged() {
    emit changed();
}

void TaskProgressPanelImpl::slotStartedChanged(bool state) {
    if (state) {
        startTime->setDateTime(QDateTime::currentDateTime());
        percentFinished->setValue(0);
    }
    enableWidgets();
}


void TaskProgressPanelImpl::slotFinishedChanged(bool state) {
    if (state) {
        percentFinished->setValue(100);
        if (!finishTime->dateTime().isValid()) {
            finishTime->setDateTime(QDateTime::currentDateTime());
        }
    }   
    enableWidgets();
}


void TaskProgressPanelImpl::enableWidgets() {
    started->setEnabled(!finished->isChecked());
    finished->setEnabled(started->isChecked());
    finishTime->setEnabled(started->isChecked());
    startTime->setEnabled(started->isChecked() && !finished->isChecked());
    performedGroup->setEnabled(started->isChecked() && !finished->isChecked());
    
    scheduledStart->setEnabled(false);
    scheduledFinish->setEnabled(false);
    scheduledEffort->setEnabled(false);
}


void TaskProgressPanelImpl::slotPercentFinishedChanged( int value ) {
    if (value == 100) {
        //remainingEffort->setValue(Duration::zeroDuration); //FIXME
    }
}


}  //KPlato namespace

#include "kpttaskprogresspanel.moc"
