/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qbuttongroup.h>

#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

#include "kptmainprojectdialog.h"
#include "kptproject.h"
#include "kptresourcespanel.h"
#include "kptresource.h"


KPTMainProjectDialog::KPTMainProjectDialog(KPTProject &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Project Settings"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    dia = new KPTMainProjectDialogImpl(this);
    resourcesTab = new KPTResourcesPanel(dia, &project);
    dia->daTabs->insertTab(resourcesTab, i18n("Resources"), 1);
    setMainWidget(dia);
    enableButtonOK(false);

	dia->namefield->setText(project.name());
	dia->leaderfield->setText(project.leader());

    dia->startDateTime->setDateTime(project.startTime());
    dia->endDateTime->setDateTime(project.endTime());
    if (project.constraint() == KPTNode::MustStartOn) {
        dia->schedulingGroup->setButton(0);
        dia->startDateTime->setEnabled(true);
    }
    else if (project.constraint() == KPTNode::MustFinishOn) {
        dia->schedulingGroup->setButton(1);
        dia->endDateTime->setEnabled(true);
    }

    connect(dia, SIGNAL( obligatedFieldsFilled(bool) ), SLOT( enableButtonOK(bool) ));

    dia->namefield->setFocus();

    connect(resourcesTab, SIGNAL( changed() ), dia, SLOT( slotCheckAllFieldsFilled() ));
}


void KPTMainProjectDialog::slotOk() {

    project.setConstraint(KPTNode::MustStartOn); // default
    if (dia->bEndDate->state())
        project.setConstraint(KPTNode::MustFinishOn);

    if (project.constraint() == KPTNode::MustStartOn)
        project.setStartTime(dia->startDateTime->dateTime());
    else if (project.constraint() == KPTNode::MustFinishOn)
        project.setEndTime(dia->endDateTime->dateTime());

    project.setName(dia->namefield->text());
    project.setLeader(dia->leaderfield->text());
    project.setDescription(dia->descriptionfield->text());

	resourcesTab->ok();

    accept();
}

KPTMainProjectDialogImpl::KPTMainProjectDialogImpl (QWidget *parent) : KPTMainProjectDialogBase(parent) {
    startDateTime->setEnabled(false);
    endDateTime->setEnabled(false);

    connect (namefield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
    connect (leaderfield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
	connect (chooseLeader, SIGNAL(pressed()), this, SLOT(slotChooseLeader()));

    connect (bStartDate, SIGNAL(clicked()), SLOT(slotStartDateClicked()) );
    connect (bEndDate, SIGNAL(clicked()), SLOT(slotEndDateClicked()) );

    connect (startDateTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotCheckAllFieldsFilled()));
    connect (endDateTime, SIGNAL(valueChanged(const QDateTime &)), SLOT(slotCheckAllFieldsFilled()));
}

void KPTMainProjectDialogImpl::slotCheckAllFieldsFilled() {
    kdDebug()<<k_funcinfo<<endl;
    emit obligatedFieldsFilled( !(namefield->text().isEmpty() || leaderfield->text().isEmpty()));
}

void KPTMainProjectDialogImpl::slotChooseLeader() {
  KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
  if (!a.isEmpty()) {
	  leaderfield->setText(a.fullEmail());
  }
}

void KPTMainProjectDialogImpl::slotStartDateClicked() {
    kdDebug()<<k_funcinfo<<endl;
    startDateTime->setEnabled(true);
    endDateTime->setEnabled(false);
    slotCheckAllFieldsFilled();
}

void KPTMainProjectDialogImpl::slotEndDateClicked() {
    kdDebug()<<k_funcinfo<<"state="<<bEndDate->state()<<" on="<<bEndDate->isOn()<<" down="<<bEndDate->isDown()<<endl;
    startDateTime->setEnabled(false);
    endDateTime->setEnabled(true);
    slotCheckAllFieldsFilled();
}


#include "kptmainprojectdialog.moc"
