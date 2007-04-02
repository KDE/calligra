/* This file is part of the KDE project
   Copyright (C) 2005-2007 Dag Andersen <danders@get2net.dk>

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

#include <kdatetimewidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <k3command.h>

#include <kdebug.h>

#include "kpttask.h"
#include "kptcommand.h"

namespace KPlato
{

MilestoneProgressPanel::MilestoneProgressPanel(Task &task, QWidget *parent, const char *name)
    : MilestoneProgressPanelImpl(parent, name),
      m_task(task),
      m_completion( task.completion() )

{
    kDebug()<<k_funcinfo<<endl;
    finished->setChecked(m_completion.isFinished());
    finishTime->setDateTime(m_completion.finishTime().dateTime());
    enableWidgets();
    finished->setFocus();
}


bool MilestoneProgressPanel::ok() {
    return true;
}

K3Command *MilestoneProgressPanel::buildCommand(Part *part) {
    K3MacroCommand *cmd = 0;
    QString c = i18n("Modify milestone completion");
    
    if ( m_completion.isFinished() != finished->isChecked() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishedCmd(part, m_completion, finished->isChecked()) );
    }
    if ( m_completion.finishTime().dateTime() != finishTime->dateTime() ) {
        if ( cmd == 0 ) cmd = new K3MacroCommand( c );
        cmd->addCommand( new ModifyCompletionFinishTimeCmd(part, m_completion, finishTime->dateTime() ) );
    }
    if ( finished->isChecked() && finishTime->dateTime().isValid() ) {
        Completion::Entry *e = new Completion::Entry( 100, Duration::zeroDuration, Duration::zeroDuration );
        cmd->addCommand( new AddCompletionEntryCmd( part, m_completion, finishTime->dateTime().date(), e ) );
    } else {
        // TODO: Remove ??
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

    connect(finishTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    
}

void MilestoneProgressPanelImpl::slotChanged() {
    emit changed();
}

void MilestoneProgressPanelImpl::slotFinishedChanged(bool state) {
    if (state) {
        if (!finishTime->dateTime().isValid()) {
            finishTime->setDateTime(QDateTime::currentDateTime());
        }
    }   
    enableWidgets();
}


void MilestoneProgressPanelImpl::enableWidgets() {
    finished->setEnabled(true);
    finishTime->setEnabled(finished->isChecked());
}


}  //KPlato namespace

#include "kptmilestoneprogresspanel.moc"
