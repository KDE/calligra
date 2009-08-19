/* This file is part of the KDE project
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptsummarytaskgeneralpanel.h"
#include "kptsummarytaskdialog.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kpttaskdescriptiondialog.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QPushButton>
#include <kdebug.h>

namespace KPlato
{

SummaryTaskGeneralPanel::SummaryTaskGeneralPanel(Task &task, QWidget *p, const char *n)
    : QWidget(p),
      m_task(task)
{
    setObjectName(n);
    setupUi(this);
    
    m_description = new TaskDescriptionPanel( task, this );
    m_description->namefield->hide();
    m_description->namelabel->hide();
    layout()->addWidget( m_description );

    QString s = i18n( "The Work Breakdown Structure introduces numbering for all tasks in the project, according to the task structure.\nThe WBS code is auto-generated.\nYou can define the WBS code pattern using the Define WBS Pattern command in the Tools menu." );
    wbslabel->setWhatsThis( s );
    wbsfield->setWhatsThis( s );

    setStartValues(task);
    
    connect(namefield, SIGNAL(textChanged(const QString&)), SLOT(slotObligatedFieldsFilled()));
    connect(leaderfield, SIGNAL(textChanged(const QString&)), SLOT(slotObligatedFieldsFilled()));
    connect(m_description, SIGNAL(textChanged(bool)), SLOT(slotObligatedFieldsFilled()));
    
    connect(chooseLeader, SIGNAL(clicked()), SLOT(slotChooseResponsible()));

}

void SummaryTaskGeneralPanel::setStartValues(Task &task) {
    namefield->setText(task.name());
    leaderfield->setText(task.leader());

    m_description->descriptionfield->setTextOrHtml(task.description());
    wbsfield->setText(task.wbsCode());
    
    namefield->setFocus();
    
}

void SummaryTaskGeneralPanel::slotObligatedFieldsFilled() {
    emit obligatedFieldsFilled(!namefield->text().isEmpty());
}

MacroCommand *SummaryTaskGeneralPanel::buildCommand() {
    MacroCommand *cmd = new MacroCommand(i18n("Modify Task"));
    bool modified = false;

    if (!namefield->isHidden() && m_task.name() != namefield->text()) {
        cmd->addCommand(new NodeModifyNameCmd(m_task, namefield->text()));
        modified = true;
    }
    if (!leaderfield->isHidden() && m_task.leader() != leaderfield->text()) {
        cmd->addCommand(new NodeModifyLeaderCmd(m_task, leaderfield->text()));
        modified = true;
    }
/*    if (!descriptionfield->isHidden() && 
        m_task.description() != descriptionfield->text()) {
        cmd->addCommand(new NodeModifyDescriptionCmd(m_task, descriptionfield->text()));
        modified = true;
    }*/
    MacroCommand *m = m_description->buildCommand();
    if ( m ) {
        cmd->addCommand( m );
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool SummaryTaskGeneralPanel::ok() {
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
