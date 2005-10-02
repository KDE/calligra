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

void KPTTaskGeneralPanelBase::setSchedulingType(int type)

{
    enableDateTime(type);
    scheduleType->setCurrentItem(type);
    emit schedulingTypeChanged(type);
 
}

int KPTTaskGeneralPanelBase::schedulingType() const 
{
    return scheduleType->currentItem();
}

void KPTTaskGeneralPanelBase::changeLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}

void KPTTaskGeneralPanelBase::setEstimationType( int type )
{
    estimateType->setCurrentItem(type);
}

int KPTTaskGeneralPanelBase::estimationType() const
{
    return estimateType->currentItem();
}

void KPTTaskGeneralPanelBase::setOptimistic( int value )
{
    optimisticValue->setValue(value);
}

void KPTTaskGeneralPanelBase::setPessimistic( int value )
{
    pessimisticValue->setValue(value);
}

int KPTTaskGeneralPanelBase::optimistic() const
{
    return optimisticValue->value();
}

int KPTTaskGeneralPanelBase::pessimistic()
{
    return pessimisticValue->value();
}

void KPTTaskGeneralPanelBase::enableDateTime( int scheduleType )
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


void KPTTaskGeneralPanelBase::estimationTypeChanged( int type )
{
    checkAllFieldsFilled();
}



void KPTTaskGeneralPanelBase::setEstimate( const KPTDuration & duration)
{
    estimate->setValue( duration );
}


void KPTTaskGeneralPanelBase::setEstimateType( int type)
{
    estimateType->setCurrentItem(type);
}


void KPTTaskGeneralPanelBase::checkAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty());
}


KPTDuration KPTTaskGeneralPanelBase::estimationValue()
{
    return estimate->value();
}


void KPTTaskGeneralPanelBase::setEstimateFields( int mask )
{
    estimate->setVisibleFields(mask);
}


void KPTTaskGeneralPanelBase::setEstimateScales( int day )
{
    estimate->setFieldScale(0, day);
    estimate->setFieldRightscale(0, day);
    
    estimate->setFieldLeftscale(1, day);
}


void KPTTaskGeneralPanelBase::setEstimateFieldUnit( int field, QString unit )
{
    estimate->setFieldUnit(field, unit);
}

void KPTTaskGeneralPanelBase::startDateChanged()
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

void KPTTaskGeneralPanelBase::startTimeChanged( const QTime &time )
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


void KPTTaskGeneralPanelBase::endDateChanged()
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

void KPTTaskGeneralPanelBase::endTimeChanged( const QTime &time )
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

void KPTTaskGeneralPanelBase::scheduleTypeChanged( int value )
{
     estimationTypeChanged(estimateType->currentItem());
}


QDateTime KPTTaskGeneralPanelBase::startDateTime()
{
    return QDateTime(startDate(), startTime());
}


QDateTime KPTTaskGeneralPanelBase::endDateTime()
{
    return QDateTime(endDate(), endTime());
}

void KPTTaskGeneralPanelBase::setStartTime( const QTime &time )
{
    scheduleStartTime->setTime(time);
}

void KPTTaskGeneralPanelBase::setEndTime( const QTime &time )
{
    scheduleEndTime->setTime(time);
}

QTime KPTTaskGeneralPanelBase::startTime() const
{
    return scheduleStartTime->time();
}

QTime KPTTaskGeneralPanelBase::endTime()
{
    return scheduleEndTime->time();
}

QDate KPTTaskGeneralPanelBase::startDate()
{
    return scheduleStartDate->date();
}


QDate KPTTaskGeneralPanelBase::endDate()
{
    return scheduleEndDate->date();
}

void KPTTaskGeneralPanelBase::setStartDateTime( const QDateTime &dt )
{
    setStartDate(dt.date());
    setStartTime(dt.time());
}


void KPTTaskGeneralPanelBase::setEndDateTime( const QDateTime &dt )
{
    setEndDate(dt.date());
    setEndTime(dt.time());
}

void KPTTaskGeneralPanelBase::setStartDate( const QDate &date )
{
    scheduleStartDate->setDate(date);
}


void KPTTaskGeneralPanelBase::setEndDate( const QDate &date )
{
    scheduleEndDate->setDate(date);
}

}  //KPlato namespace
