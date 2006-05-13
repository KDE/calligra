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

#include <QCheckBox>
#include <q3buttongroup.h>
#include <qdatetime.h>
#include <q3datetimeedit.h>
#include <qradiobutton.h>
#include <QPushButton>

#include <QLabel>
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
#include "kptschedule.h"

namespace KPlato
{

MainProjectPanel::MainProjectPanel(Project &p, QWidget *parent, const char *name)
    : MainProjectPanelImpl(parent, name),
      project(p)
{
    namefield->setText(project.name());
    idfield->setText(project.id());
    leaderfield->setText(project.leader());
    descriptionfield->setText(project.description());
    wbs->setText(project.wbs());

    //baseline->setChecked(project.isBaselined()); FIXME: Removed for this release

    QDateTime st = project.constraintStartTime();
    QDateTime et = project.constraintEndTime();
    QString s = i18n("Scheduling");
    Schedule *sch = project.currentSchedule();
    if (sch) {
        s = i18n("Scheduling (%1)", sch->typeToString(true));
    }
    schedulingGroup->setTitle(s);
    if (project.constraint() == Node::MustStartOn) {
        schedulingGroup->setButton(0);
        if (sch)
            et = project.endTime();
    } else if (project.constraint() == Node::MustFinishOn) {
        schedulingGroup->setButton(1);
        if (sch)
            st = project.startTime();
    } else {
        kWarning()<<k_funcinfo<<"Illegal constraint: "<<project.constraint()<<endl;
        schedulingGroup->setButton(0);
        if (sch)
            et = project.endTime();
    }
    startDate->setDate(st.date());
    startTime->setTime(st.time());
    endDate->setDate(et.date());
    endTime->setTime(et.time());
    enableDateTime();
    //slotBaseline();
    namefield->setFocus();
}


bool MainProjectPanel::ok() {
    if (idfield->text() != project.id() && project.findNode(idfield->text())) {
        KMessageBox::sorry(this, i18n("Project id must be unique"));
        idfield->setFocus();
        return false;
    }
    return true;
}

KCommand *MainProjectPanel::buildCommand(Part *part) {
    KMacroCommand *m = 0;
    QString c = i18n("Modify main project");
    if (project.name() != namefield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new NodeModifyNameCmd(part, project, namefield->text()));
    }
    if (project.id() != idfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new NodeModifyIdCmd(part, project, idfield->text()));
    }
    if (project.leader() != leaderfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new NodeModifyLeaderCmd(part, project, leaderfield->text()));
    }
    if (project.description() != descriptionfield->text()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new NodeModifyDescriptionCmd(part, project, descriptionfield->text()));
    }
/*  FIXME: Removed for this release
    if (baseline->isChecked() != project.isBaselined()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new ProjectModifyBaselineCmd(part, project, baseline->isChecked()));
    } */
    if (bStartDate->isChecked() && project.constraint() != Node::MustStartOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new ProjectModifyConstraintCmd(part, project, Node::MustStartOn));
    }
    if (bEndDate->isChecked() && project.constraint() != Node::MustFinishOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new ProjectModifyConstraintCmd(part, project, Node::MustFinishOn));
    }
    if (bStartDate->isChecked() && startDateTime() != project.constraintStartTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new ProjectModifyStartTimeCmd(part, project, startDateTime()));
    }
    if (bEndDate->isChecked() && endDateTime() != project.constraintEndTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new ProjectModifyEndTimeCmd(part, project, endDateTime()));
    }
    return m;
}

//-------------------------------------------------------------------
MainProjectPanelImpl::MainProjectPanelImpl(QWidget *parent, const char *name)
    :  MainProjectPanelBase(parent, name) {

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
    //connect( baseline, SIGNAL( toggled(bool) ), this, SLOT( slotCheckAllFieldsFilled() ) ); FIXME: Removed for this release
    connect( namefield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( idfield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( leaderfield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    //connect( baseline, SIGNAL( toggled(bool) ), this, SLOT( slotBaseline() ) ); FIXME: Removed for this release
    connect( chooseLeader, SIGNAL( clicked() ), this, SLOT( slotChooseLeader() ) );
}

void MainProjectPanelImpl::slotCheckAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !idfield->text().isEmpty() && !leaderfield->text().isEmpty());
}


void MainProjectPanelImpl::slotChooseLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}


void MainProjectPanelImpl::slotStartDateClicked()
{
    enableDateTime();
}


void MainProjectPanelImpl::slotEndDateClicked()
{
    enableDateTime();
}



void MainProjectPanelImpl::enableDateTime()
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


QDateTime MainProjectPanelImpl::startDateTime()
{
    return QDateTime(startDate->date(), startTime->time());
}


QDateTime MainProjectPanelImpl::endDateTime()
{
    return QDateTime(endDate->date(), endTime->time());
}


void MainProjectPanelImpl::slotBaseline()
{
    bool b = false;
    //b = baseline->isChecked(); FIXME: Removed for this release
    namefield->setReadOnly(b);
    idfield->setReadOnly(b);
    leaderfield->setReadOnly(b);
    chooseLeader->setEnabled(!b);
    schedulingGroup->setEnabled(!b);
}

}  //KPlato namespace

#include "kptmainprojectpanel.moc"
