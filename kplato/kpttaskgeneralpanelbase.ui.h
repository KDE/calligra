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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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

void KPTTaskGeneralPanelBase::setStartTime( const QDateTime &dt )
{
    scheduleStartTime->setDateTime(dt);
}

void KPTTaskGeneralPanelBase::setEndTime( const QDateTime &dt )
{
    scheduleEndTime->setDateTime(dt);
}

QDateTime KPTTaskGeneralPanelBase::startTime() const
{
    return scheduleStartTime->dateTime();
}

QDateTime KPTTaskGeneralPanelBase::endTime()
{
    return scheduleEndTime->dateTime();
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
    switch (scheduleType)
    {
    case 0: //ASAP
    case 1: //ALAP
        scheduleStartTime->setEnabled(false);
        scheduleEndTime->setEnabled(false);
        break;
    case 2: //Must start on
    case 4: // Start not earlier
        scheduleStartTime->setEnabled(true);
        scheduleEndTime->setEnabled(false);
        break;
    case 3: //Must finish on
    case 5: // Finish not later
        scheduleStartTime->setEnabled(false);
        scheduleEndTime->setEnabled(true);
        break;
    case 6: //Fixed interval
        scheduleStartTime->setEnabled(true);
        scheduleEndTime->setEnabled(true);
 estimateType->setCurrentItem(0); // Effort
 break;
    default:
        scheduleStartTime->setEnabled(false);
        scheduleEndTime->setEnabled(false);
        break;
    }
}


void KPTTaskGeneralPanelBase::estimationTypeChanged( int type )
{
    if (scheduleType->currentItem() == 6 /* Fixed interval */) {
 if (type != 0 /*Effort*/) {
             KMessageBox::sorry(this, i18n("Estimation type must be Effort\nwhen schedule type is 'Fixed interval'"));
      estimateType->setCurrentItem(0); // Stay at Effort
        }
        return;
    }
    checkAllFieldsFilled();
}



void KPTTaskGeneralPanelBase::setEstimate( const KPlato::KPTDuration & duration)
{
    estimate->setValue( duration );
}


void KPTTaskGeneralPanelBase::setEstimateType( int type)
{
    estimateType->setCurrentItem(type);
}


void KPTTaskGeneralPanelBase::checkAllFieldsFilled()
{
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty());
}


KPlato::KPTDuration KPTTaskGeneralPanelBase::estimationValue()
{
    return estimate->value();
}


void KPTTaskGeneralPanelBase::setEstimateFields( int mask )
{
    estimate->setVisibleFields(mask);
}


void KPTTaskGeneralPanelBase::setEstimateScales( int day )
{
    estimate->setFieldScale(0, day, day);
    estimate->setFieldRightscale(0, day);
    
    estimate->setFieldLeftscale(1, day);
}


void KPTTaskGeneralPanelBase::setEstimateFieldUnit( int field, QString unit )
{
    estimate->setFieldUnit(field, unit);
}


void KPTTaskGeneralPanelBase::startTimeChanged( const QDateTime &dt )
{
    if (dt > endTime()) 
    {
 scheduleEndTime->blockSignals(true);
        if (endTime().time() > dt.time())
     setEndTime(QDateTime(dt.date(), endTime().time()));
 else
           setEndTime(dt);
 scheduleEndTime->blockSignals(false);
    }
}


void KPTTaskGeneralPanelBase::endTimeChanged( const QDateTime &dt )
{
    if (dt < startTime()) 
    {
 scheduleStartTime->blockSignals(true);
 if (startTime().time() < dt.time())
            setStartTime(QDateTime(dt.date(), startTime().time()));
        else 
     setStartTime(dt);
        scheduleStartTime->blockSignals(false);
    }
}
