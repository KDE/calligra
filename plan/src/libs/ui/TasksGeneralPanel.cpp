/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#include "TasksGeneralPanel.h"
#include "kpttaskdialog.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptdurationspinbox.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptproject.h"

#include <KLocalizedString>

#ifdef PLAN_KDEPIMLIBS_FOUND
#include <akonadi/contact/emailaddressselectiondialog.h>
#include <akonadi/contact/emailaddressselectionwidget.h>
#include <akonadi/contact/emailaddressselection.h>
#endif

#include <QDateTime>

#include <kptdebug.h>

namespace KPlato
{

TasksGeneralPanel::TasksGeneralPanel(Project &project, QList<Task*> &tasks, QWidget *p, const char *n)
    : TasksGeneralPanelImpl(p, n),
      m_tasks(tasks),
      m_project(project)
{
    Q_ASSERT(!tasks.isEmpty());
    
    useTime = true;
    setStartValues( tasks.at(0) );

//     if ( task.isBaselined( BASELINESCHEDULE ) ) {
//         schedulingGroup->setEnabled( false );
//     }

}

void TasksGeneralPanel::setStartValues( Task *task ) {
    m_estimate = m_duration = task->estimate()->expectedValue();
    leaderfield->setText(task->leader());

    int cal = 0;
    m_calendars.clear();
    calendarCombo->addItem(i18n("None"));
    m_calendars.insert(0, 0);
    QList<Calendar*> list = m_project.allCalendars();
    int i=1;
    foreach (Calendar *c, list) {
        calendarCombo->insertItem(i, c->name());
        m_calendars.insert(i, c);
        if (c == task->estimate()->calendar()) {
            cal = i;
        }
        ++i;
    }
    calendarCombo->setCurrentIndex(cal);

    estimate->setMinimumUnit( (Duration::Unit)(m_project.config().minimumDurationUnit()) );
    estimate->setMaximumUnit( (Duration::Unit)(m_project.config().maximumDurationUnit()) );
    estimate->setUnit( task->estimate()->unit() );
    setEstimateType(task->estimate()->type());
    if (task->estimate()->type() == Estimate::Type_Effort && task->estimate()->expectedEstimate() == 0.0) {
        setEstimateType(2 /*Milestone*/);
    }
    setSchedulingType(task->constraint());
    if (task->constraintStartTime().isValid()) {
        setStartDateTime(task->constraintStartTime());
    } else {
        QDate date = QDate::currentDate();
        setStartDateTime(QDateTime(date, QTime(), Qt::LocalTime));
    }
    if (task->constraintEndTime().isValid()) {
        setEndDateTime(task->constraintEndTime());
    } else {
        setEndDateTime(QDateTime(startDate().addDays(1), QTime(), Qt::LocalTime));
    }
    //debugPlan<<"Estimate:"<<task->estimate()->expected().toString();
    setEstimate(task->estimate()->expectedEstimate());
    setOptimistic(task->estimate()->optimisticRatio());
    setPessimistic(task->estimate()->pessimisticRatio());
    setRisktype(task->estimate()->risktype());
    
    namefield->setFocus();
}

MacroCommand *TasksGeneralPanel::buildCommand() {
    MacroCommand *cmd = new MacroCommand(kundo2_i18n("Modify Tasks"));
    bool modified = false;

    for (Task *t : m_tasks) {
        if (!leaderfield->isHidden() && t->leader() != leaderfield->text()) {
            cmd->addCommand(new NodeModifyLeaderCmd(*t, leaderfield->text()));
            modified = true;
        }
        Node::ConstraintType c = (Node::ConstraintType)schedulingType();
        if (c != t->constraint()) {
            cmd->addCommand(new NodeModifyConstraintCmd(*t, c));
            modified = true;
        }
        if (startDateTime() != t->constraintStartTime() &&
            (c == Node::FixedInterval || c == Node::StartNotEarlier || c == Node::MustStartOn)) {
            cmd->addCommand(new NodeModifyConstraintStartTimeCmd(*t, startDateTime()));
            modified = true;
        }
        if (endDateTime() != t->constraintEndTime() &&
            (c == Node::FinishNotLater || c == Node::FixedInterval || c == Node::MustFinishOn)) {
            cmd->addCommand(new NodeModifyConstraintEndTimeCmd(*t, endDateTime()));
            modified = true;
        }
        int et = estimationType();
        if (et == 2 /*Milestome*/) {
            et = 0; /*Effort*/
        }
        if (et != t->estimate()->type()) {
            cmd->addCommand(new ModifyEstimateTypeCmd(*t,  t->estimate()->type(), et));
            modified = true;
        }
        bool unitchanged = estimate->unit() != t->estimate()->unit();
        if ( unitchanged ) {
            cmd->addCommand( new ModifyEstimateUnitCmd( *t, t->estimate()->unit(), estimate->unit() ) );
            modified = true;
        }
        bool expchanged = estimationValue() != t->estimate()->expectedEstimate();
        if ( expchanged ) {
            cmd->addCommand(new ModifyEstimateCmd(*t, t->estimate()->expectedEstimate(), estimationValue()));
            modified = true;
        }
        int x = optimistic();
        if ( x != t->estimate()->optimisticRatio() || expchanged || unitchanged ) {
            cmd->addCommand(new EstimateModifyOptimisticRatioCmd(*t, t->estimate()->optimisticRatio(), x));
            modified = true;
        }
        x = pessimistic();
        if ( x != t->estimate()->pessimisticRatio() || expchanged || unitchanged ) {
            cmd->addCommand(new EstimateModifyPessimisticRatioCmd(*t, t->estimate()->pessimisticRatio(), x));
            modified = true;
        }
        if (t->estimate()->risktype() != risktype()) {
            cmd->addCommand(new EstimateModifyRiskCmd(*t, t->estimate()->risktype(), risktype()));
            modified = true;
        }
        if (t->estimate()->calendar() != calendar()) {
            cmd->addCommand(new ModifyEstimateCalendarCmd(*t, t->estimate()->calendar(), calendar()));
            modified = true;
        }
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TasksGeneralPanel::ok() {
    return true;
}

void TasksGeneralPanel::estimationTypeChanged(int type) {
    if (type == 0 /*Effort*/) {
        estimate->setEnabled(true);
        calendarCombo->setEnabled(false);
    } else if ( type == 1 /*Duration*/ ) {
        calendarCombo->setEnabled(false);
        if (schedulingType() == 6) { /*Fixed interval*/
            estimate->setEnabled(false);
        } else {
            estimate->setEnabled(true);
            calendarCombo->setEnabled(true);
        }
    } else if ( type == 2 /* Milestone */ ) {
        estimate->setValue( 0 );
        estimate->setEnabled(false);
        calendarCombo->setEnabled(false);
    }
    TasksGeneralPanelImpl::estimationTypeChanged(type);
}

void TasksGeneralPanel::scheduleTypeChanged(int value)
{
    if (value == 6 /*Fixed interval*/) {
        if (estimateType->currentIndex() == 1/*duration*/){
//            setEstimateScales(24);
            estimate->setEnabled(false);
//TODO            setEstimate( DateTime( endDateTime(), KDateTime::UTC) - DateTime( startDateTime(), KDateTime::UTC ) );
        }
    } else {
        estimate->setEnabled(true);
    }
    TasksGeneralPanelImpl::scheduleTypeChanged(value);
}

//-----------------------------
TasksGeneralPanelImpl::TasksGeneralPanelImpl(QWidget *p, const char *n)
    : QWidget(p) {

    setObjectName(n);
    setupUi(this);

#ifndef PLAN_KDEPIMLIBS_FOUND
    chooseLeader->hide();
#endif

    // FIXME
    // [Bug 311940] New: Plan crashes when typing a text in the filter textbox before the textbook is fully loaded when selecting a contact from the adressbook
    chooseLeader->hide();

    connect(namefield, SIGNAL(textChanged(QString)), SLOT(checkAllFieldsFilled()));
    connect(leaderfield, SIGNAL(textChanged(QString)), SLOT(checkAllFieldsFilled()));
    connect(chooseLeader, SIGNAL(clicked()), SLOT(changeLeader()));
    connect(estimateType, SIGNAL(activated(int)), SLOT(estimationTypeChanged(int)));
    connect(scheduleType, SIGNAL(activated(int)), SLOT(scheduleTypeChanged(int)));
    connect(scheduleStartDate, SIGNAL(dateChanged(QDate)), SLOT(startDateChanged()));
    connect(scheduleStartTime, SIGNAL(timeChanged(QTime)), SLOT(startTimeChanged(QTime)));
    connect(scheduleEndDate, SIGNAL(dateChanged(QDate)), SLOT(endDateChanged()));
    connect(scheduleEndTime, SIGNAL(timeChanged(QTime)), SLOT(endTimeChanged(QTime)));
    connect(estimate, SIGNAL(valueChanged(double)), SLOT(checkAllFieldsFilled()));
    connect(optimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(pessimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(risk, SIGNAL(activated(int)), SLOT(checkAllFieldsFilled()));
    connect(calendarCombo, SIGNAL(activated(int)), SLOT(calendarChanged(int)));

}

void TasksGeneralPanelImpl::setSchedulingType(int type)
{
    enableDateTime(type);
    scheduleType->setCurrentIndex(type);
    emit schedulingTypeChanged(type);
}

int TasksGeneralPanelImpl::schedulingType() const
{
    return scheduleType->currentIndex();
}

void TasksGeneralPanelImpl::changeLeader()
{
#ifdef PLAN_KDEPIMLIBS_FOUND
    QPointer<Akonadi::EmailAddressSelectionDialog> dlg = new Akonadi::EmailAddressSelectionDialog( this );
    if ( dlg->exec() && dlg ) {
        QStringList names;
        const Akonadi::EmailAddressSelection::List selections = dlg->selectedAddresses();
        foreach ( const Akonadi::EmailAddressSelection &selection, selections ) {
            QString s = selection.name();
            if ( ! selection.email().isEmpty() ) {
                if ( ! selection.name().isEmpty() ) {
                    s += " <";
                }
                s += selection.email();
                if ( ! selection.name().isEmpty() ) {
                    s += '>';
                }
                if ( ! s.isEmpty() ) {
                    names << s;
                }
            }
        }
        if ( ! names.isEmpty() ) {
            leaderfield->setText( names.join( ", " ) );
        }
    }
#endif
}

void TasksGeneralPanelImpl::setEstimationType( int type )
{
    estimateType->setCurrentIndex(type);
}

int TasksGeneralPanelImpl::estimationType() const
{
    return estimateType->currentIndex();
}

void TasksGeneralPanelImpl::setOptimistic( int value )
{
    optimisticValue->setValue(value);
}

void TasksGeneralPanelImpl::setPessimistic( int value )
{
    pessimisticValue->setValue(value);
}

int TasksGeneralPanelImpl::optimistic() const
{
    return optimisticValue->value();
}

int TasksGeneralPanelImpl::pessimistic()
{
    return pessimisticValue->value();
}

void TasksGeneralPanelImpl::enableDateTime( int scheduleType )
{
    scheduleStartTime->setEnabled(false);
    scheduleEndTime->setEnabled(false);
    scheduleStartDate->setEnabled(false);
    scheduleEndDate->setEnabled(false);
    switch (scheduleType)
    {
    case 0: //ASAP
    case 1: //ALAP
        break;
    case 2: //Must start on
    case 4: // Start not earlier
        if (useTime) {
            scheduleStartTime->setEnabled(true);
            scheduleEndTime->setEnabled(false);
        }
        scheduleStartDate->setEnabled(true);
        scheduleEndDate->setEnabled(false);
        break;
    case 3: //Must finish on
    case 5: // Finish not later
        if (useTime) {
            scheduleStartTime->setEnabled(false);
            scheduleEndTime->setEnabled(true);
        }
        scheduleStartDate->setEnabled(false);
        scheduleEndDate->setEnabled(true);
        break;
    case 6: //Fixed interval
        if (useTime) {
            scheduleStartTime->setEnabled(true);
            scheduleEndTime->setEnabled(true);
        }
        scheduleStartDate->setEnabled(true);
        scheduleEndDate->setEnabled(true);
        break;
    default:
        break;
    }
}


void TasksGeneralPanelImpl::estimationTypeChanged( int /*type*/ )
{
    checkAllFieldsFilled();
}

void TasksGeneralPanelImpl::calendarChanged( int /*index*/ )
{
    checkAllFieldsFilled();
}

void TasksGeneralPanelImpl::setEstimate( double duration)
{
    estimate->setValue( duration );
}


void TasksGeneralPanelImpl::setEstimateType( int type)
{
    estimateType->setCurrentIndex(type);
    estimationTypeChanged( type );
}


void TasksGeneralPanelImpl::checkAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(true); // do not block save even if name is not filled
}


double TasksGeneralPanelImpl::estimationValue()
{
    return estimate->value();
}

void TasksGeneralPanelImpl::startDateChanged()
{
    if (!scheduleStartDate->isEnabled()) {
        return;
    }
    QDate date = startDate();
    if (startDateTime() > endDateTime())
    {
        scheduleEndTime->blockSignals(true);
        scheduleEndDate->blockSignals(true);
        setEndDate(date);
        setEndTime(startTime());
        scheduleEndTime->blockSignals(false);
        scheduleEndDate->blockSignals(false);
    }
    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TasksGeneralPanelImpl::startTimeChanged( const QTime &time )
{
    if (!scheduleStartTime->isEnabled()) {
        return;
    }
    if (startDateTime() > endDateTime())
    {
        scheduleEndTime->blockSignals(true);
        setEndTime(time);
        scheduleEndTime->blockSignals(false);
    }
    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}


void TasksGeneralPanelImpl::endDateChanged()
{
    if (!scheduleEndDate->isEnabled()) {
        return;
    }
    QDate date = endDate();
    if (endDateTime() < startDateTime())
    {
        scheduleStartTime->blockSignals(true);
        scheduleStartDate->blockSignals(true);
        setStartDate(date);
        setStartTime(endTime());
        scheduleStartTime->blockSignals(false);
        scheduleStartDate->blockSignals(false);
    }

    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TasksGeneralPanelImpl::endTimeChanged( const QTime &time )
{
    if (!scheduleEndTime->isEnabled()) {
        return;
    }
    if (endDateTime() < startDateTime())
    {
        scheduleStartTime->blockSignals(true);
        setStartTime(time);
        scheduleStartTime->blockSignals(false);
    }

    if (scheduleType->currentIndex() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentIndex());
    }
    checkAllFieldsFilled();
}

void TasksGeneralPanelImpl::scheduleTypeChanged( int value )
{
     estimationTypeChanged(estimateType->currentIndex());
     enableDateTime(value);
     checkAllFieldsFilled();
}


QDateTime TasksGeneralPanelImpl::startDateTime()
{
    return QDateTime(startDate(), startTime(), Qt::LocalTime);
}


QDateTime TasksGeneralPanelImpl::endDateTime()
{
    return QDateTime(endDate(), endTime(), Qt::LocalTime);
}

void TasksGeneralPanelImpl::setStartTime( const QTime &time )
{
    scheduleStartTime->setTime( QTime( time.hour(), time.minute(), 0 ) );
}

void TasksGeneralPanelImpl::setEndTime( const QTime &time )
{
    scheduleEndTime->setTime( QTime( time.hour(), time.minute(), 0 ) );
}

QTime TasksGeneralPanelImpl::startTime() const
{
    QTime t = scheduleStartTime->time();
    t.setHMS( t.hour(), t.minute(), 0 );
    return t;
}

QTime TasksGeneralPanelImpl::endTime()
{
    QTime t = scheduleEndTime->time();
    t.setHMS( t.hour(), t.minute(), 0 );
    return t;
}

QDate TasksGeneralPanelImpl::startDate()
{
    return scheduleStartDate->date();
}


QDate TasksGeneralPanelImpl::endDate()
{
    return scheduleEndDate->date();
}

void TasksGeneralPanelImpl::setStartDateTime( const QDateTime &dt )
{
    setStartDate(dt.date());
    setStartTime(dt.time());
}


void TasksGeneralPanelImpl::setEndDateTime( const QDateTime &dt )
{
    setEndDate(dt.date());
    setEndTime(dt.time());
}

void TasksGeneralPanelImpl::setStartDate( const QDate &date )
{
    scheduleStartDate->setDate(date);
}


void TasksGeneralPanelImpl::setEndDate( const QDate &date )
{
    scheduleEndDate->setDate(date);
}

void TasksGeneralPanelImpl::setRisktype( int r )
{
    risk->setCurrentIndex(r);
}

int TasksGeneralPanelImpl::risktype() const
{
    return risk->currentIndex();
}

Calendar *TasksGeneralPanelImpl::calendar() const
{
    return m_calendars.value( calendarCombo->currentIndex() );
}

}  //KPlato namespace
