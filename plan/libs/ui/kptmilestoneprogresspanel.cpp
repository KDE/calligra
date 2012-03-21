/* This file is part of the KDE project
   Copyright (C) 2005-2007, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptmilestoneprogresspanel.h"

#include <QCheckBox>
#include <QDateTime>

#include <klocale.h>
#include <kmessagebox.h>

#include "kpttask.h"
#include "kptcommand.h"
#include "kptdebug.h"


namespace KPlato
{

MilestoneProgressPanel::MilestoneProgressPanel(Task &task, QWidget *parent, const char *name)
    : MilestoneProgressPanelImpl(parent, name),
      m_task(task),
      m_completion( task.completion() )

{
    kDebug(planDbg());
    finished->setChecked(m_completion.isFinished());
    finishTime->setDateTime(m_completion.finishTime());
    enableWidgets();
    finished->setFocus();
}


MacroCommand *MilestoneProgressPanel::buildCommand() {
    MacroCommand *cmd = 0;
    QString c = i18n("Modify milestone completion");
    
    if ( m_completion.isFinished() != finished->isChecked() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartedCmd( m_completion, finished->isChecked()) );
        cmd->addCommand( new ModifyCompletionFinishedCmd( m_completion, finished->isChecked()) );
    }
    if ( m_completion.finishTime() != finishTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionStartTimeCmd( m_completion, finishTime->dateTime() ) );
        cmd->addCommand( new ModifyCompletionFinishTimeCmd( m_completion, finishTime->dateTime() ) );
    }
    if ( finished->isChecked() && finishTime->dateTime().isValid() ) {
        if ( cmd == 0 ) cmd = new MacroCommand( c );
        cmd->addCommand( new ModifyCompletionPercentFinishedCmd( m_completion, finishTime->dateTime().date(), 100 ) );
    } else {
        foreach( const QDate &date, m_completion.entries().keys() ) {
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            cmd->addCommand( new RemoveCompletionEntryCmd( m_completion, date ) );
        }
    }
    return cmd;
}

//-------------------------------------

MilestoneProgressPanelImpl::MilestoneProgressPanelImpl(QWidget *parent, const char *name)
    : QWidget(parent) {
    
    setObjectName(name);
    setupUi(this);
    
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotChanged()));

    connect(finishTime, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotChanged()));
    
}

void MilestoneProgressPanelImpl::slotChanged() {
    emit changed();
}

void MilestoneProgressPanelImpl::slotFinishedChanged(bool state) {
    if (state) {
        finishTime->setDateTime(QDateTime::currentDateTime());
    }
    enableWidgets();
}


void MilestoneProgressPanelImpl::enableWidgets() {
    finished->setEnabled(true);
    finishTime->setEnabled(finished->isChecked());
}


}  //KPlato namespace

#include "kptmilestoneprogresspanel.moc"
