/* This file is part of the KDE project
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptmainprojectpanel.h"

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qradiobutton.h>
#include <qpushbutton.h>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatewidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcommand.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

#include "kptproject.h"
#include "kptcommand.h"

namespace KPlato
{

KPTMainProjectPanel::KPTMainProjectPanel(KPTProject &p, QWidget *parent, const char *name)
    : KPTMainProjectPanelImpl(parent, name),
      project(p)
{
    namefield->setText(project.name());
    idfield->setText(project.id());
    leaderfield->setText(project.leader());
    descriptionfield->setText(project.description());

    baseline->setChecked(project.isBaselined());
    
    startDate->setDate(project.startTime().date());
    endDate->setDate(project.endTime().date());
    if (project.constraint() == KPTNode::MustStartOn) {
        schedulingGroup->setButton(0);
    }
    else if (project.constraint() == KPTNode::MustFinishOn) {
        schedulingGroup->setButton(1);
    } else {
        kdWarning()<<k_funcinfo<<"Illegal constraint: "<<project.constraint()<<endl;
        schedulingGroup->setButton(0);
    }    
    enableDateTime();
    slotBaseline();
    namefield->setFocus();
}


bool KPTMainProjectPanel::ok() {
    if (idfield->text() != project.id() && project.findNode(idfield->text())) {
        KMessageBox::sorry(this, i18n("Project id must be unique"));
        idfield->setFocus();
        return false;
    }
    return true;
}

KCommand *KPTMainProjectPanel::buildCommand(KPTPart *part) {
    KMacroCommand *m = 0;
    QString c = i18n("Modify main project");
    if (project.name() != namefield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyNameCmd(part, project, namefield->text()));
    }
    if (project.id() != idfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyIdCmd(part, project, idfield->text()));
    }
    if (project.leader() != leaderfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyLeaderCmd(part, project, leaderfield->text()));
    }
    if (project.description() != descriptionfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyDescriptionCmd(part, project, descriptionfield->text()));
    }
    if (baseline->isChecked() != project.isBaselined()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTProjectModifyBaselineCmd(part, project, baseline->isChecked()));
    } 
    if (bStartDate->state() && project.constraint() != KPTNode::MustStartOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyConstraintCmd(part, project, KPTNode::MustStartOn));
    } 
    if (bEndDate->state() && project.constraint() != KPTNode::MustFinishOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyConstraintCmd(part, project, KPTNode::MustFinishOn));
    } 
    if (startDateTime() != project.startTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyStartTimeCmd(part, project, startDateTime()));
    }
    if (endDateTime() != project.endTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyEndTimeCmd(part, project, endDateTime()));
    }
    return m;
}

//-------------------------------------------------------------------
KPTMainProjectPanelImpl::KPTMainProjectPanelImpl(QWidget *parent, const char *name)
    :  KPTMainProjectPanelBase(parent, name) {

    // signals and slots connections
    connect( bStartDate, SIGNAL( clicked() ), this, SLOT( slotStartDateClicked() ) );
    connect( bEndDate, SIGNAL( clicked() ), this, SLOT( slotEndDateClicked() ) );
    connect( bStartDate, SIGNAL( clicked() ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( bEndDate, SIGNAL( clicked() ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( descriptionfield, SIGNAL( textChanged() ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( endDate, SIGNAL( changed(QDate) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( endTime, SIGNAL( valueChanged(const QTime&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( startDate, SIGNAL( changed(QDate) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( startTime, SIGNAL( valueChanged(const QTime&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( baseline, SIGNAL( toggled(bool) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( namefield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( idfield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( leaderfield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( baseline, SIGNAL( toggled(bool) ), this, SLOT( slotBaseline() ) );
    connect( chooseLeader, SIGNAL( clicked() ), this, SLOT( slotChooseLeader() ) );
}

void KPTMainProjectPanelImpl::slotCheckAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty() && !leaderfield->text().isEmpty());
}


void KPTMainProjectPanelImpl::slotChooseLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty()) 
    {
        leaderfield->setText(a.fullEmail());
    }
}


void KPTMainProjectPanelImpl::slotStartDateClicked()
{
    enableDateTime();    
}


void KPTMainProjectPanelImpl::slotEndDateClicked()
{
    enableDateTime();
}



void KPTMainProjectPanelImpl::enableDateTime()
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


QDateTime KPTMainProjectPanelImpl::startDateTime()
{
    return QDateTime(startDate->date(), startTime->time());
}


QDateTime KPTMainProjectPanelImpl::endDateTime()
{
    return QDateTime(endDate->date(), endTime->time());
}


void KPTMainProjectPanelImpl::slotBaseline()
{
    bool b = baseline->isChecked();
    namefield->setReadOnly(b);
    idfield->setReadOnly(b);
    leaderfield->setReadOnly(b);
    chooseLeader->setEnabled(!b);
    schedulingGroup->setEnabled(!b);
}

}  //KPlato namespace

#include "kptmainprojectpanel.moc"
