/* This file is part of the KDE project
   Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

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

#include "kptsummarytaskgeneralpanel.h"
#include "kptsummarytaskdialog.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptconfig.h"
#include "kptpart.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kcommand.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <qdatetime.h> 
#include <q3datetimeedit.h> 
#include <q3groupbox.h> 
#include <kdebug.h>

namespace KPlato
{

SummaryTaskGeneralPanel::SummaryTaskGeneralPanel(Task &task, QWidget *p, const char *n)
    : SummaryTaskGeneralPanelBase(p, n),
      m_task(task)
{
    setStartValues(task);
    
    connect(namefield, SIGNAL(textChanged(const QString&)), SLOT(slotObligatedFieldsFilled()));
    connect(leaderfield, SIGNAL(textChanged(const QString&)), SLOT(slotObligatedFieldsFilled()));
    connect(idfield, SIGNAL(textChanged(const QString&)), SLOT(slotObligatedFieldsFilled()));
    connect(descriptionfield, SIGNAL(textChanged()), SLOT(slotObligatedFieldsFilled()));
    
    connect(chooseLeader, SIGNAL(clicked()), SLOT(slotChooseResponsible()));

}

void SummaryTaskGeneralPanel::setStartValues(Task &task) {
    namefield->setText(task.name());
    leaderfield->setText(task.leader());
    descriptionfield->setText(task.description());
    idfield->setText(task.id());
    wbsfield->setText(task.wbs());
    
    namefield->setFocus();
    
}

void SummaryTaskGeneralPanel::slotObligatedFieldsFilled() {
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty());
}

KMacroCommand *SummaryTaskGeneralPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task"));
    bool modified = false;

    if (!namefield->isHidden() && m_task.name() != namefield->text()) {
        cmd->addCommand(new NodeModifyNameCmd(part, m_task, namefield->text()));
        modified = true;
    }
    if (!leaderfield->isHidden() && m_task.leader() != leaderfield->text()) {
        cmd->addCommand(new NodeModifyLeaderCmd(part, m_task, leaderfield->text()));
        modified = true;
    }
    if (!descriptionfield->isHidden() && 
        m_task.description() != descriptionfield->text()) {
        cmd->addCommand(new NodeModifyDescriptionCmd(part, m_task, descriptionfield->text()));
        modified = true;
    }
    if (!idfield->isHidden() && idfield->text() != m_task.id()) {
        cmd->addCommand(new NodeModifyIdCmd(part, m_task, idfield->text()));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool SummaryTaskGeneralPanel::ok() {
    if (idfield->text() != m_task.id() && m_task.findNode(idfield->text())) {
        KMessageBox::sorry(this, i18n("Task id must be unique"));
        idfield->setFocus();
        return false;
    }
    return true;
}

void SummaryTaskGeneralPanel::slotChooseResponsible() {
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty()) {
        leaderfield->setText(a.fullEmail());
        leaderfield->setFocus();
    }
}


}  //KPlato namespace

#include "kptsummarytaskgeneralpanel.moc"
