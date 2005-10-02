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

#include "kptduration.h"

namespace KPlato 
{

void KPTTaskProgressPanelBase::slotChanged()
{
    emit changed();
}



void KPTTaskProgressPanelBase::slotStartedChanged(bool state)
{
    if (state)
    {
        startTime->setDateTime(QDateTime::currentDateTime());
 percentFinished->setValue(0);
    }
    enableWidgets();
}


void KPTTaskProgressPanelBase::slotFinishedChanged(bool state)
{
    if (state)
    {
        percentFinished->setValue(100);
 if (!finishTime->dateTime().isValid()) 
 {
            finishTime->setDateTime(QDateTime::currentDateTime());
        }
    }   
    enableWidgets();
}


void KPTTaskProgressPanelBase::enableWidgets()
{
    started->setEnabled(!finished->isChecked());
    finished->setEnabled(started->isChecked());
    finishTime->setEnabled(started->isChecked());
    startTime->setEnabled(started->isChecked() && !finished->isChecked());
    performedGroup->setEnabled(started->isChecked() && !finished->isChecked());
    
    scheduledStart->setEnabled(false);
    scheduledFinish->setEnabled(false);
    scheduledEffort->setEnabled(false);
}


void KPTTaskProgressPanelBase::slotPercentFinishedChanged( int value )
{
    if (value == 100)
    {
 //remainingEffort->setValue(KPTDuration::zeroDuration); //FIXME
    }
}

}
