/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@suse.de

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpttaskdialog.h"
#include "kptrequestresourcespanel.h"
#include "kpttask.h"
#include "kptcommand.h"

#include <qlayout.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdatepicker.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>
#include <kcommand.h>

#include <qtextedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qtable.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <kdebug.h>

KPTTaskDialog::KPTTaskDialog(KPTTask &task, QPtrList<KPTResourceGroup> &resourceGroups, QWidget *p, const char *n)
    : KDialogBase(Tabbed, i18n("Task Settings"), Ok|Cancel, Ok, p, n, true, true),
      task(task)
{
    dia = new KPTTaskDialogImpl(this);
    resourcesTab = new KPTRequestResourcesPanel(dia, task, resourceGroups);
    dia->daTabs->insertTab(resourcesTab, i18n("Resources"), 1);
    setMainWidget(dia);
    enableButtonOK(false);

	dia->namefield->setText(task.name());
	dia->leaderfield->setText(task.leader());

    dia->setScheduling(task.constraint());
    if (task.constraintTime().isValid())
        dia->setSchedulerDateTime(task.constraintTime());
    else
        dia->setSchedulerDateTime(QDateTime::currentDateTime());

    connect(dia, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));

    dia->namefield->setFocus();

    connect(resourcesTab, SIGNAL( changed() ), dia, SLOT( slotCheckAllFieldsFilled() ));

    resize(900,500); //FIXME: design ui so we don't need this

}


KMacroCommand *KPTTaskDialog::buildCommand() {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify task"));
    bool modified = false;
    
    KPTDuration dt = KPTDuration();

    if (task.name() != dia->namefield->text()) {
        cmd->addCommand(new KPTNodeModifyNameCmd(task, dia->namefield->text()));
        modified = true;
    }
    if (task.leader() != dia->leaderfield->text()) {
        cmd->addCommand(new KPTNodeModifyLeaderCmd(task, dia->leaderfield->text()));
        modified = true;
    }
    if (task.description() != dia->descriptionfield->text()) {
        cmd->addCommand(new KPTNodeModifyDescriptionCmd(task, dia->descriptionfield->text()));
        modified = true;
    }
    KPTNode::ConstraintType c = (KPTNode::ConstraintType)dia->scheduling();
    if (c != task.constraint()) {
        cmd->addCommand(new KPTNodeModifyConstraintCmd(task, c));
        modified = true;
    }
    if (c == KPTNode::FinishNotLater || c == KPTNode::StartNotEarlier || c == KPTNode::MustStartOn) {
        cmd->addCommand(new KPTNodeModifyConstraintTimeCmd(task, dia->schedulerDateTime()));
        modified = true;
    }
    KMacroCommand *m = resourcesTab->buildCommand();
    if (m) {
        cmd->addCommand(m);
        modified = true;
    }
    if (modified)
        return cmd;
      
    delete cmd;
    return 0;
}

void KPTTaskDialog::slotOk() {
    accept();
}

//////////////////////////////////////////

KPTTaskDialogImpl::KPTTaskDialogImpl (QWidget *parent) : KPTTaskDialogBase(parent) {
    connect (namefield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
    connect (leaderfield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
    connect (scheduleType, SIGNAL(clicked( int )), this, SLOT(slotSchedulingChanged( int )) );
    connect (chooseLeader, SIGNAL(pressed()), SLOT(slotChooseLeader()));
}

void KPTTaskDialogImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled( !(namefield->text().isEmpty() || leaderfield->text().isEmpty()));
}

void KPTTaskDialogImpl::slotSchedulingChanged(int activated) {
    emit schedulingTypeChanged(activated);
    slotCheckAllFieldsFilled();
}

void KPTTaskDialogImpl::slotChooseLeader()
{
  KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
  if (!a.isEmpty()) {
	  leaderfield->setText(a.fullEmail());
  }
}

int KPTTaskDialogImpl::scheduling() const
{
    return scheduleType->selectedId();
}

void KPTTaskDialogImpl::setScheduling(int type)
{
    scheduleType->setButton(type);
}

QDateTime KPTTaskDialogImpl::schedulerDateTime() const
{
    return QDateTime(schedulerDate->getDate(), schedulerTime->time());
}

void KPTTaskDialogImpl::setSchedulerDateTime(QDateTime dt)
{
    schedulerDate->setDate(dt.date());
    schedulerTime->setTime(dt.time());
}

#include "kpttaskdialog.moc"
