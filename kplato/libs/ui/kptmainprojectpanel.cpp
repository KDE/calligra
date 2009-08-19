/* This file is part of the KDE project
   Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>

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

#include "kptmainprojectpanel.h"

#include <QCheckBox>
#include <QPushButton>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatewidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

#include "kptproject.h"
#include "kptcommand.h"
#include "kptschedule.h"
#include "kpttaskdescriptiondialog.h"

namespace KPlato
{

MainProjectPanel::MainProjectPanel(Project &p, QWidget *parent)
    : QWidget(parent),
      project(p)
{
    setupUi(this);

    QString s = i18n( "The Work Breakdown Structure introduces numbering for all tasks in the project, according to the task structure.\nThe WBS code is auto-generated.\nYou can define the WBS code pattern using the Define WBS Pattern command in the Tools menu." );
    wbslabel->setWhatsThis( s );
    wbs->setWhatsThis( s );

    namefield->setText(project.name());
    leaderfield->setText(project.leader());
    m_description = new TaskDescriptionPanel( p, this );
    m_description->namefield->hide();
    m_description->namelabel->hide();
    layout()->addWidget( m_description );

    wbs->setText(project.wbsCode());
    if ( wbs->text().isEmpty() ) {
        wbslabel->hide();
        wbs->hide();
    }

    DateTime st = project.constraintStartTime();
    DateTime et = project.constraintEndTime();
    startDate->setDate(st.date());
    startTime->setTime(st.time());
    endDate->setDate(et.date());
    endTime->setTime(et.time());
    enableDateTime();
    namefield->setFocus();

    // signals and slots connections
    connect( m_description, SIGNAL( textChanged(bool) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( endDate, SIGNAL( dateChanged(const QDate&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( endTime, SIGNAL( timeChanged(const QTime&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( startDate, SIGNAL( dateChanged(const QDate&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( startTime, SIGNAL( timeChanged(const QTime&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( namefield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( leaderfield, SIGNAL( textChanged(const QString&) ), this, SLOT( slotCheckAllFieldsFilled() ) );
    connect( chooseLeader, SIGNAL( clicked() ), this, SLOT( slotChooseLeader() ) );
}


bool MainProjectPanel::ok() {
    return true;
}

MacroCommand *MainProjectPanel::buildCommand() {
    MacroCommand *m = 0;
    QString c = i18n("Modify main project");
    if (project.name() != namefield->text()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new NodeModifyNameCmd(project, namefield->text()));
    }
    if (project.leader() != leaderfield->text()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new NodeModifyLeaderCmd(project, leaderfield->text()));
    }
    if (startDateTime() != project.constraintStartTime().dateTime()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new ProjectModifyStartTimeCmd(project, startDateTime()));
    }
    if (endDateTime() != project.constraintEndTime().dateTime()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new ProjectModifyEndTimeCmd(project, endDateTime()));
    }
    MacroCommand *cmd = m_description->buildCommand();
    if ( cmd ) {
        if (!m) m = new MacroCommand(c);
        m->addCommand( cmd );
    }
    return m;
}

void MainProjectPanel::slotCheckAllFieldsFilled()
{
    emit changed();
    emit obligatedFieldsFilled(!namefield->text().isEmpty() && !leaderfield->text().isEmpty());
}


void MainProjectPanel::slotChooseLeader()
{
    KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
    if (!a.isEmpty())
    {
        leaderfield->setText(a.fullEmail());
    }
}


void MainProjectPanel::slotStartDateClicked()
{
    enableDateTime();
}


void MainProjectPanel::slotEndDateClicked()
{
    enableDateTime();
}



void MainProjectPanel::enableDateTime()
{
    kDebug();
    startTime->setEnabled(true);
    startDate->setEnabled(true);
    endTime->setEnabled(true);
    endDate->setEnabled(true);
}


QDateTime MainProjectPanel::startDateTime()
{
    return QDateTime(startDate->date(), startTime->time());
}


QDateTime MainProjectPanel::endDateTime()
{
    return QDateTime(endDate->date(), endTime->time());
}


}  //KPlato namespace

#include "kptmainprojectpanel.moc"
