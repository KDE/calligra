/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
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

namespace KPlato
{

void TaskGeneralPanelBase::setSchedulingType(int type)

{
    enableDateTime(type);
    scheduleType->setCurrentItem(type);
    emit schedulingTypeChanged(type);
 
}

int TaskGeneralPanelBase::schedulingType() const 
{
    return scheduleType->currentItem();
}

void TaskGeneralPanelBase::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}

void TaskGeneralPanelBase::setEstimationType( int type )
{
    estimateType->setCurrentItem(type);
}

int TaskGeneralPanelBase::estimationType() const
{
    return estimateType->currentItem();
}

void TaskGeneralPanelBase::setOptimistic( int value )
{
    optimisticValue->setValue(value);
}

void TaskGeneralPanelBase::setPessimistic( int value )
{
    pessimisticValue->setValue(value);
}

int TaskGeneralPanelBase::optimistic() const
{
    return optimisticValue->value();
}

int TaskGeneralPanelBase::pessimistic()
{
    return pessimisticValue->value();
}

void TaskGeneralPanelBase::enableDateTime( int scheduleType )
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


void TaskGeneralPanelBase::estimationTypeChanged( int type )
{
    checkAllFieldsFilled();
}



void TaskGeneralPanelBase::setEstimate( const Duration & duration)
{
    estimate->setValue( duration );
}


void TaskGeneralPanelBase::setEstimateType( int type)
{
    estimateType->setCurrentItem(type);
}


void TaskGeneralPanelBase::checkAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty());
}


Duration TaskGeneralPanelBase::estimationValue()
{
    return estimate->value();
}


void TaskGeneralPanelBase::setEstimateFields( int mask )
{
    estimate->setVisibleFields(mask);
}


void TaskGeneralPanelBase::setEstimateScales( int day )
{
    estimate->setFieldScale(0, day);
    estimate->setFieldRightscale(0, day);
    
    estimate->setFieldLeftscale(1, day);
}


void TaskGeneralPanelBase::setEstimateFieldUnit( int field, QString unit )
{
    estimate->setFieldUnit(field, unit);
}

void TaskGeneralPanelBase::startDateChanged()
{
    if (!scheduleStartDate->isEnabled()) {
        return;
    }
    QDate date = startDate();
    if (date > endDate()) 
    {
        scheduleEndTime->blockSignals(true);
        setEndDate(date);
        scheduleEndTime->blockSignals(false);
    }
    if (scheduleType->currentItem() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentItem());
    }
}

void TaskGeneralPanelBase::startTimeChanged( const QTime &time )
{
    if (!scheduleStartTime->isEnabled()) {
        return;
    }
    if (time > endTime()) 
    {
        scheduleEndTime->blockSignals(true);
        setEndTime(time);
        scheduleEndTime->blockSignals(false);
    }
    if (scheduleType->currentItem() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentItem());
    }
    
}


void TaskGeneralPanelBase::endDateChanged()
{
    if (!scheduleEndDate->isEnabled()) {
        return;
    }
    QDate date = endDate();
    if (date < startDate()) 
    {
        scheduleStartTime->blockSignals(true);
        setStartDate(date);
        scheduleStartTime->blockSignals(false);
    }
    
    if (scheduleType->currentItem() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentItem());
    }
}

void TaskGeneralPanelBase::endTimeChanged( const QTime &time )
{
    if (!scheduleEndTime->isEnabled()) {
        return;
    }
    if (time < startTime()) 
    {
        scheduleStartTime->blockSignals(true);
        setStartTime(time);
        scheduleStartTime->blockSignals(false);
    }
    
    if (scheduleType->currentItem() == 6 /*FixedInterval*/)
    {
        estimationTypeChanged(estimateType->currentItem());
    }
}

void TaskGeneralPanelBase::scheduleTypeChanged( int value )
{
     estimationTypeChanged(estimateType->currentItem());
}


QDateTime TaskGeneralPanelBase::startDateTime()
{
    return QDateTime(startDate(), startTime());
}


QDateTime TaskGeneralPanelBase::endDateTime()
{
    return QDateTime(endDate(), endTime());
}

void TaskGeneralPanelBase::setStartTime( const QTime &time )
{
    scheduleStartTime->setTime(time);
}

void TaskGeneralPanelBase::setEndTime( const QTime &time )
{
    scheduleEndTime->setTime(time);
}

QTime TaskGeneralPanelBase::startTime() const
{
    return scheduleStartTime->time();
}

QTime TaskGeneralPanelBase::endTime()
{
    return scheduleEndTime->time();
}

QDate TaskGeneralPanelBase::startDate()
{
    return scheduleStartDate->date();
}


QDate TaskGeneralPanelBase::endDate()
{
    return scheduleEndDate->date();
}

void TaskGeneralPanelBase::setStartDateTime( const QDateTime &dt )
{
    setStartDate(dt.date());
    setStartTime(dt.time());
}


void TaskGeneralPanelBase::setEndDateTime( const QDateTime &dt )
{
    setEndDate(dt.date());
    setEndTime(dt.time());
}

void TaskGeneralPanelBase::setStartDate( const QDate &date )
{
    scheduleStartDate->setDate(date);
}


void TaskGeneralPanelBase::setEndDate( const QDate &date )
{
    scheduleEndDate->setDate(date);
}

}  //KPlato namespace
