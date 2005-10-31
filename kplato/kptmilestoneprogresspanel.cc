/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptmilestoneprogresspanel.h"

#include <qcheckbox.h>

#include <kdatetimewidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcommand.h>

#include <kdebug.h>

#include "kpttask.h"
#include "kptcommand.h"

namespace KPlato
{

KPTMilestoneProgressPanel::KPTMilestoneProgressPanel(KPTTask &task, QWidget *parent, const char *name)
    : KPTMilestoneProgressPanelImpl(parent, name),
      m_task(task)
{
    kdDebug()<<k_funcinfo<<endl;
    m_progress = task.progress();
    finished->setChecked(m_progress.finished);
    finishTime->setDateTime(m_progress.finishTime);
        
    enableWidgets();
    finished->setFocus();
}


bool KPTMilestoneProgressPanel::ok() {
    m_progress.started = finished->isChecked();
    m_progress.finished = finished->isChecked();
    m_progress.startTime = finishTime->dateTime();
    m_progress.finishTime = finishTime->dateTime();
    m_progress.percentFinished = m_progress.finished ? 100 : 0;
    return true;
}

KCommand *KPTMilestoneProgressPanel::buildCommand(KPTPart *part) {
    KCommand *cmd = 0;
    QString c = i18n("Modify progress");
    if (m_task.progress() != m_progress) {
        cmd = new KPTTaskModifyProgressCmd(part, m_task, m_progress, c);
    }
    return cmd;
}

//-------------------------------------

KPTMilestoneProgressPanelImpl::KPTMilestoneProgressPanelImpl(QWidget *parent, const char *name, WFlags f)
    : KPTMilestoneProgressPanelBase(parent, name, f) {
    
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotFinishedChanged(bool)));
    connect(finished, SIGNAL(toggled(bool)), SLOT(slotChanged()));

    connect(finishTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotChanged()));
    
}

void KPTMilestoneProgressPanelImpl::slotChanged() {
    emit changed();
}

void KPTMilestoneProgressPanelImpl::slotFinishedChanged(bool state) {
    if (state) {
        if (!finishTime->dateTime().isValid()) {
            finishTime->setDateTime(QDateTime::currentDateTime());
        }
    }   
    enableWidgets();
}


void KPTMilestoneProgressPanelImpl::enableWidgets() {
    finished->setEnabled(true);
    finishTime->setEnabled(finished->isChecked());
}


}  //KPlato namespace

#include "kptmilestoneprogresspanel.moc"
