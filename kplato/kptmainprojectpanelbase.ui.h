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


namespace KPlato 
{
    
    void KPTMainProjectPanelBase::slotCheckAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty() && !leaderfield->text().isEmpty());
}


void KPTMainProjectPanelBase::slotChooseLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty()) 
    {
        leaderfield->setText(a.fullEmail());
    }
}


void KPTMainProjectPanelBase::slotStartDateClicked()
{
    enableDateTime();    
}


void KPTMainProjectPanelBase::slotEndDateClicked()
{
    enableDateTime();
}



void KPTMainProjectPanelBase::enableDateTime()
{
    if (schedulingGroup->selected() == bStartDate)
    {
        startTime->setEnabled(true);
        startDate->setEnabled(true);
        endTime->setEnabled(false);
        endDate->setEnabled(false);
    }
    if (schedulingGroup->selected() == bEndDate)
    {
        startTime->setEnabled(false);
        startDate->setEnabled(false);
        endTime->setEnabled(true);
        endDate->setEnabled(true);
    }
}


QDateTime KPTMainProjectPanelBase::startDateTime()
{
    return QDateTime(startDate->date(), startTime->time());
}


QDateTime KPTMainProjectPanelBase::endDateTime()
{
    return QDateTime(endDate->date(), endTime->time());
}


void KPTMainProjectPanelBase::slotBaseline()
{
    bool b = baseline->isChecked();
    namefield->setReadOnly(b);
    idfield->setReadOnly(b);
    leaderfield->setReadOnly(b);
    chooseLeader->setEnabled(!b);
    schedulingGroup->setEnabled(!b);
}

} // namespace KPlato

