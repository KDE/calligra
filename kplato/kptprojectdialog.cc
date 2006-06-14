/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk

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

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <q3textedit.h>
#include <QLineEdit>
#include <q3datetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <q3textbrowser.h>

#include <kdatepicker.h>
#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

#include "kptprojectdialog.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptprojectdialogbase.h"
#include "kptresourcespanel.h"

namespace KPlato
{

ProjectDialog::ProjectDialog(Project &p, QWidget *parent, const char *name)
    : KDialog( parent),
      project(p)
{
    setCaption( i18n("Project Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonSeparator( true );
    dia = new ProjectDialogImpl(this);
    resourcesTab = new ResourcesPanel(dia, &project);
    dia->daTabs->insertTab(resourcesTab, i18n("Resources"), 1);
    setMainWidget(dia);
    enableButtonOK(false);

	dia->namefield->setText(project.name());
	dia->leaderfield->setText(project.leader());

    connect(dia, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(dia, SIGNAL( schedulingTypeChanged(int) ), this, SLOT( slotSchedulingChanged(int) ));

    slotSchedulingChanged(dia->schedulerType->currentItem());
    dia->namefield->setFocus();

    connect(resourcesTab, SIGNAL( changed() ), dia, SLOT( slotCheckAllFieldsFilled() ));
}


void ProjectDialog::slotOk() {
    project.setConstraint((Node::ConstraintType) dia->schedulerType->currentItem());
    //FIXME
    project.setStartTime(QDateTime(dia->schedulerDate->date(), dia->schedulerTime->time()));
    project.setConstraintStartTime(QDateTime(dia->schedulerDate->date(), dia->schedulerTime->time()));

    project.setName(dia->namefield->text());
    project.setLeader(dia->leaderfield->text());
    project.setDescription(dia->descriptionfield->text());

	resourcesTab->ok();

    accept();
}

void ProjectDialog::slotSchedulingChanged(int activated) {
    bool needDate = activated >= 2;
    dia->schedulerTime->setEnabled(needDate);
    dia->schedulerDate->setEnabled(needDate);

    QString label = QString("<p><font size=\"4\" color=\"#7797BC\"><b>%1</b></font></p><p>%2</p>");
    switch(activated) {
        // TODO please provide nice explenations on this.
        case 0: // ASAP
            label = label.arg(i18n("As Soon as Possible"));
            label = label.arg(i18n("Place all events at the earliest possible moment permitted in the schedule"));
            break;
        case 1: // ALAP
            label = label.arg(i18n("As Late as Possible"));
            label = label.arg(i18n("Place all events at the last possible moment permitted in the schedule"));
            break;
        case 2: // Start not earlier then
            label = label.arg(i18n("Start not Earlier then"));
            label = label.arg(i18n(""));
            break;
        case 3: // Finish not later then
            label = label.arg(i18n("Finish not Later then"));
            label = label.arg(i18n(""));
            break;
        case 4: // Must start on
            label = label.arg(i18n("Must Start on"));
            label = label.arg(i18n(""));
            break;
        default: // error ...
            dia->lSchedulingExplain->setText("");
            return;
    }
    dia->lSchedulingExplain->setText(label);
}

ProjectDialogImpl::ProjectDialogImpl (QWidget *parent) : ProjectDialogBase(parent) {
    connect (namefield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
    connect (leaderfield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFieldsFilled()) );
    connect (schedulerType, SIGNAL(activated( int )), this, SLOT(slotSchedulingChanged( int )) );
	connect (chooseLeader, SIGNAL(pressed()), this, SLOT(slotChooseLeader()));
}

void ProjectDialogImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled( !(namefield->text().isEmpty() || leaderfield->text().isEmpty()));
}

void ProjectDialogImpl::slotSchedulingChanged(int activated) {
    emit schedulingTypeChanged(activated);
}

void ProjectDialogImpl::slotChooseLeader()
{
  KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
  if (!a.isEmpty()) {
	  leaderfield->setText(a.fullEmail());
  }
}

}  //KPlato namespace

#include "kptprojectdialog.moc"
