/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskdefaultpanel.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptdurationspinbox.h"
#include "kptcalendar.h"
#include "kptdatetime.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>
#include <kdatewidget.h>

#include <QDateTime>
#include <QPushButton>

#include <kdebug.h>

namespace KPlato
{

TaskDefaultPanel::TaskDefaultPanel(Task &task, QWidget *parent, const char *n)
    : ConfigTaskPanelImpl(parent, n),
      m_task(task)
{
    setStartValues(task);
}

void TaskDefaultPanel::setStartValues(Task &task) {
    m_estimate = m_duration = task.estimate()->expectedValue();
    leaderfield->setText(task.leader());
    descriptionfield->setText(task.description());

    estimate->setUnit( task.estimate()->unit() );
    setEstimateType(task.estimate()->type());

    setSchedulingType(task.constraint());
    if (task.constraintStartTime().isValid()) {
        setStartDateTime(task.constraintStartTime().dateTime());
    } else {
        QDate date = QDate::currentDate();
        setStartDateTime(QDateTime(date, QTime()));
    }
    if (task.constraintEndTime().isValid()) {
        setEndDateTime(task.constraintEndTime().dateTime());
    } else {
        setEndDateTime(QDateTime(startDate().addDays(1), QTime()));
    }
    //kDebug()<<"Estimate:"<<task.estimate()->expected().toString();
    setEstimate(task.estimate()->expectedEstimate());
    setOptimistic(task.estimate()->optimisticRatio());
    setPessimistic(task.estimate()->pessimisticRatio());

    leaderfield->setFocus();
}

MacroCommand *TaskDefaultPanel::buildCommand() {
    MacroCommand *cmd = new MacroCommand(i18n("Modify Default Task"));
    bool modified = false;

    if (m_task.leader() != leaderfield->text()) {
        cmd->addCommand(new NodeModifyLeaderCmd(m_task, leaderfield->text()));
        modified = true;
    }
    if (m_task.description() != descriptionfield->text()) {
        cmd->addCommand(new NodeModifyDescriptionCmd(m_task, descriptionfield->text()));
        modified = true;
    }
    Node::ConstraintType c = (Node::ConstraintType)schedulingType();
    if (c != m_task.constraint()) {
        cmd->addCommand(new NodeModifyConstraintCmd(m_task, c));
        modified = true;
    }
    if (startDateTime() != m_task.constraintStartTime().dateTime()) {
        cmd->addCommand(new NodeModifyConstraintStartTimeCmd(m_task, startDateTime()));
        modified = true;
    }
    if (endDateTime() != m_task.constraintEndTime().dateTime()) {
        cmd->addCommand(new NodeModifyConstraintEndTimeCmd(m_task, endDateTime()));
        modified = true;
    }
    int et = estimationType();
    if (et != m_task.estimate()->type()) {
        cmd->addCommand(new ModifyEstimateTypeCmd(m_task,  m_task.estimate()->type(), et));
        modified = true;
    }
    bool unitchanged = estimate->unit() != m_task.estimate()->unit();
    if ( unitchanged ) {
        cmd->addCommand( new ModifyEstimateUnitCmd( m_task, m_task.estimate()->unit(), estimate->unit() ) );
        modified = true;
    }
    bool expchanged = estimationValue() != m_task.estimate()->expectedEstimate();
    if ( expchanged ) {
        cmd->addCommand(new ModifyEstimateCmd(m_task, m_task.estimate()->expectedEstimate(), estimationValue()));
        modified = true;
    }
    int x = optimistic();
    kDebug()<<x<<m_task.estimate()->optimisticRatio();
    if ( x != m_task.estimate()->optimisticRatio() || expchanged || unitchanged ) {
        cmd->addCommand(new EstimateModifyOptimisticRatioCmd(m_task, m_task.estimate()->optimisticRatio(), x));
        modified = true;
    }
    x = pessimistic();
    kDebug()<<x<<m_task.estimate()->pessimisticRatio();
    if ( x != m_task.estimate()->pessimisticRatio() || expchanged || unitchanged ) {
        cmd->addCommand(new EstimateModifyPessimisticRatioCmd(m_task, m_task.estimate()->pessimisticRatio(), x));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskDefaultPanel::ok() {
    return true;
}

void TaskDefaultPanel::estimationTypeChanged(int type) {
    if (type == 0 /*Effort*/) {
/*        Duration d = estimationValue();
        setEstimateScales(m_dayLength);*/
        //setEstimate(d);
    } else if (type == 1 /*Duration*/) {
/*        Duration d = estimationValue();
        setEstimateScales(24);*/
        //setEstimate(d);
    } else /* Length */ {
    }
    ConfigTaskPanelImpl::estimationTypeChanged(type);
}

void TaskDefaultPanel::scheduleTypeChanged(int value)
{
    if (value == 6 /*Fixed interval*/) {
        if (estimateType->currentIndex() == 1/*duration*/){
//            setEstimateScales(24);
            //estimate->setEnabled(false);
            //setEstimate(DateTime(endDateTime()) - DateTime(startDateTime()));
        }
    } else {
//        setEstimateScales(m_dayLength);
        estimate->setEnabled(true);
    }
    ConfigTaskPanelImpl::scheduleTypeChanged(value);
}


//-----------------------------
ConfigTaskPanelImpl::ConfigTaskPanelImpl(QWidget *p, const char *n)
    : QWidget(p) {

    setObjectName(n);
    setupUi(this);

    connect(leaderfield, SIGNAL(textChanged(const QString &)), SLOT(checkAllFieldsFilled()));
    connect(chooseLeader, SIGNAL(clicked()), SLOT(changeLeader()));
    connect(estimateType, SIGNAL(activated(int)), SLOT(estimationTypeChanged(int)));
    connect(scheduleType, SIGNAL(activated(int)), SLOT(scheduleTypeChanged(int)));
    connect(scheduleStartDate, SIGNAL(changed(QDate)), SLOT(startDateChanged()));
    connect(scheduleStartTime, SIGNAL(timeChanged(const QTime&)), SLOT(startTimeChanged(const QTime&)));
    connect(scheduleEndDate, SIGNAL(changed(QDate)), SLOT(endDateChanged()));
    connect(scheduleEndTime, SIGNAL(timeChanged(const QTime&)), SLOT(endTimeChanged(const QTime&)));
    connect(estimate, SIGNAL(valueChanged(double)), SLOT(checkAllFieldsFilled()));
    connect(optimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(pessimisticValue, SIGNAL(valueChanged(int)), SLOT(checkAllFieldsFilled()));
    connect(descriptionfield, SIGNAL(textChanged()), SLOT(checkAllFieldsFilled()));
}

void ConfigTaskPanelImpl::setSchedulingType(int type)
{
    enableDateTime(type);
    scheduleType->setCurrentIndex(type);
    emit schedulingTypeChanged(type);
}

int ConfigTaskPanelImpl::schedulingType() const
{
    return scheduleType->currentIndex();
}

void ConfigTaskPanelImpl::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}

void ConfigTaskPanelImpl::setEstimationType( int type )
{
    estimateType->setCurrentIndex(type);
}

int ConfigTaskPanelImpl::estimationType() const
{
    return estimateType->currentIndex();
}

void ConfigTaskPanelImpl::setOptimistic( int value )
{
    optimisticValue->setValue(value);
}

void ConfigTaskPanelImpl::setPessimistic( int value )
{
    pessimisticValue->setValue(value);
}

int ConfigTaskPanelImpl::optimistic() const
{
    return optimisticValue->value();
}

int ConfigTaskPanelImpl::pessimistic()
{
    return pessimisticValue->value();
}

void ConfigTaskPanelImpl::enableDateTime( int /*scheduleType*/ )
{
    scheduleStartTime->setEnabled(true);
    scheduleEndTime->setEnabled(true);
    scheduleStartDate->setEnabled(true);
    scheduleEndDate->setEnabled(true);
/*    switch (scheduleType)
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
    }*/
}


void ConfigTaskPanelImpl::estimationTypeChanged( int /*type*/ )
{
    checkAllFieldsFilled();
}



void ConfigTaskPanelImpl::setEstimate( double duration)
{
    estimate->setValue( duration );
}


void ConfigTaskPanelImpl::setEstimateType( int type)
{
    estimateType->setCurrentIndex(type);
}


void ConfigTaskPanelImpl::checkAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(true);
}


double ConfigTaskPanelImpl::estimationValue()
{
    return estimate->value();
}

void ConfigTaskPanelImpl::startDateChanged()
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

void ConfigTaskPanelImpl::startTimeChanged( const QTime &time )
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


void ConfigTaskPanelImpl::endDateChanged()
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

void ConfigTaskPanelImpl::endTimeChanged( const QTime &time )
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

void ConfigTaskPanelImpl::scheduleTypeChanged( int value )
{
     estimationTypeChanged(estimateType->currentIndex());
     enableDateTime(value);
     checkAllFieldsFilled();
}


QDateTime ConfigTaskPanelImpl::startDateTime()
{
    return QDateTime(startDate(), startTime());
}


QDateTime ConfigTaskPanelImpl::endDateTime()
{
    return QDateTime(endDate(), endTime());
}

void ConfigTaskPanelImpl::setStartTime( const QTime &time )
{
    scheduleStartTime->setTime(time);
}

void ConfigTaskPanelImpl::setEndTime( const QTime &time )
{
    scheduleEndTime->setTime(time);
}

QTime ConfigTaskPanelImpl::startTime() const
{
    return scheduleStartTime->time();
}

QTime ConfigTaskPanelImpl::endTime()
{
    return scheduleEndTime->time();
}

QDate ConfigTaskPanelImpl::startDate()
{
    return scheduleStartDate->date();
}


QDate ConfigTaskPanelImpl::endDate()
{
    return scheduleEndDate->date();
}

void ConfigTaskPanelImpl::setStartDateTime( const QDateTime &dt )
{
    setStartDate(dt.date());
    setStartTime(dt.time());
}


void ConfigTaskPanelImpl::setEndDateTime( const QDateTime &dt )
{
    setEndDate(dt.date());
    setEndTime(dt.time());
}

void ConfigTaskPanelImpl::setStartDate( const QDate &date )
{
    scheduleStartDate->setDate(date);
}


void ConfigTaskPanelImpl::setEndDate( const QDate &date )
{
    scheduleEndDate->setDate(date);
}


}  //KPlato namespace

#include "kpttaskdefaultpanel.moc"
