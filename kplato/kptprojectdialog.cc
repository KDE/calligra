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

#include "kptprojectdialog.h"
#include "kptproject.h"
#include "kptresource.h"

#include <kdebug.h>

#include <kdatepicker.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <klocale.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>

KPTProjectDialog::KPTProjectDialog(KPTProject &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Project Settings"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    dia = new KPTProjectDialogImpl(this);
    resourcesTab = new KPTResourcesPanel(dia, &project);
    dia->daTabs->insertTab(resourcesTab, i18n("Resources"), 1);
    setMainWidget(dia);
    enableButtonOK(false);

    connect(dia, SIGNAL( obligatedFieldsFilled(bool) ), this, SLOT( enableButtonOK(bool) ));
    connect(dia, SIGNAL( schedulingTypeChanged(int) ), this, SLOT( slotSchedulingChanged(int) ));

    slotSchedulingChanged(dia->schedulerType->currentItem());
    dia->namefield->setFocus();
}


void KPTProjectDialog::slotOk() {
    KPTDuration dt = KPTDuration();

    project.setConstraint((KPTNode::ConstraintType) dia->schedulerType->currentItem());
    QDateTime endTime = QDateTime(dia->schedulerDate->date(), dia->schedulerTime->time());
    dt.set(endTime);
    project.setStartTime(dt);
    project.setMustStartOn(dt);

    project.setName(dia->namefield->text());
    project.setLeader(dia->leaderfield->text());
    project.setDescription(dia->descriptionfield->text());

    accept();
}

void KPTProjectDialog::slotSchedulingChanged(int activated) {
    bool needDate = activated >= 2;
    dia->schedulerTime->setEnabled(needDate);
    dia->schedulerDate->setEnabled(needDate);

    QString label = QString("<p><font size=\"4\" color=\"#7797BC\"><b>%1</b></font><br>%2</p>");
    switch(activated) {
        // TODO please provide nice explenations on this.
        case 0: // ASAP
            label = label.arg(i18n("As Soon As Possible"));
            label = label.arg(i18n(""));
            break;
        case 1: // ALAP
            label = label.arg(i18n("ALAP ?"));
            label = label.arg(i18n(""));
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

KPTProjectDialogImpl::KPTProjectDialogImpl (QWidget *parent) : KPTProjectDialogBase(parent) {
    connect (namefield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFiedsFilled()) );
    connect (leaderfield, SIGNAL(textChanged( const QString& )), this, SLOT(slotCheckAllFiedsFilled()) );
    connect (schedulerType, SIGNAL(activated( int )), this, SLOT(slotSchedulingChanged( int )) );
}

void KPTProjectDialogImpl::slotCheckAllFiedsFilled() {
    emit obligatedFieldsFilled( !(namefield->text().isEmpty() || leaderfield->text().isEmpty()));
}

void KPTProjectDialogImpl::slotSchedulingChanged(int activated) {
    emit schedulingTypeChanged(activated);
}

#include "kptprojectdialog.moc"
