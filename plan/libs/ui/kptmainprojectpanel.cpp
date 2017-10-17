/* This file is part of the KDE project
   Copyright (C) 2004-2007, 2011, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptdebug.h"

#include <KLocalizedString>

#ifdef PLAN_KDEPIMLIBS_FOUND
#include <akonadi/contact/emailaddressselectiondialog.h>
#include <akonadi/contact/emailaddressselectionwidget.h>
#include <akonadi/contact/emailaddressselection.h>
#endif

#include "kptproject.h"
#include "kptcommand.h"
#include "kptschedule.h"
#include "kpttaskdescriptiondialog.h"

#include <QFileDialog>

namespace KPlato
{

MainProjectPanel::MainProjectPanel(Project &p, QWidget *parent)
    : QWidget(parent),
      project(p)
{
    setupUi(this);

#ifndef PLAN_KDEPIMLIBS_FOUND
    chooseLeader->hide();
#endif

    // FIXME
    // [Bug 311940] New: Plan crashes when typing a text in the filter textbox before the textbook is fully loaded when selecting a contact from the adressbook
    chooseLeader->hide();

    QString s = i18n( "The Work Breakdown Structure introduces numbering for all tasks in the project, according to the task structure.\nThe WBS code is auto-generated.\nYou can define the WBS code pattern using the Define WBS Pattern command in the Tools menu." );
    wbslabel->setWhatsThis( s );
    wbs->setWhatsThis( s );

    namefield->setText(project.name());
    leaderfield->setText(project.leader());
 //   useSharedResources->setEnabled(!project.isSharedResourcesLoaded());
    useSharedResources->setChecked(project.useSharedResources());
    resourcesFile->setText(project.sharedResourcesFile());
    projectsPlace->setText(project.sharedProjectsUrl().toDisplayString());
    qInfo()<<Q_FUNC_INFO<<project.sharedProjectsUrl();
    m_description = new TaskDescriptionPanel( p, tabWidget->widget(1) );
    m_description->namefield->hide();
    m_description->namelabel->hide();

    wbs->setText(project.wbsCode());
    if ( wbs->text().isEmpty() ) {
        wbslabel->hide();
        wbs->hide();
    }

    DateTime st = project.constraintStartTime();
    DateTime et = project.constraintEndTime();
    startDate->setDate(st.date());
    startTime->setTime( QTime( st.time().hour(), st.time().minute(), 0 ) );
    endDate->setDate(et.date());
    endTime->setTime( QTime( et.time().hour(), et.time().minute(), 0 ) );
    enableDateTime();
    namefield->setFocus();

    useSharedResources->setToolTip(xi18nc("@info:tooltip", "Enables sharing resources with other projects"));
    useSharedResources->setWhatsThis(xi18nc("@info:whatsthis",
                                           "<title>Shared resources</title>"
                                           "Resources can be shared between projects"
                                           " to avoid overbooking resources across projects."
                                           " Shared resources must be defined in a separate file,"
                                           " and you must have at least read access to it."
                                           " The projects that share the resources must also be"
                                           " accessible by you."
                                           ));
    s = xi18nc("@info:tooltip", "File where shared resources are defined");
    resourcesLabel->setToolTip(s);
    resourcesType->setToolTip(s);
    resourcesFile->setToolTip(s);

    s = xi18nc("@info:tooltip", "Directory where all the projects that share resources can be found");
    projectsLabel->setToolTip(s);
    projectsType->setToolTip(s);
    projectsPlace->setToolTip(s);

    projectsLoadAtStartup->setChecked(project.loadProjectsAtStartup());
    projectsLoadAtStartup->setToolTip(xi18nc("@info:tooltip", "Load shared resource assignments at startup"));

    projectsLoadBtn->setToolTip(xi18nc("@info:tooltip", "Load (or re-load) shared resource assignments"));
    projectsClearBtn->setToolTip(xi18nc("@info:tooltip", "Clear shared resource assignments"));

    // signals and slots connections
    connect( m_description, SIGNAL(textChanged(bool)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( endDate, SIGNAL(dateChanged(QDate)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( endTime, SIGNAL(timeChanged(QTime)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( startDate, SIGNAL(dateChanged(QDate)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( startTime, SIGNAL(timeChanged(QTime)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( namefield, SIGNAL(textChanged(QString)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( leaderfield, SIGNAL(textChanged(QString)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( useSharedResources, SIGNAL(toggled(bool)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( resourcesFile, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect( projectsPlace, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckAllFieldsFilled()) );
    connect(projectsLoadAtStartup, SIGNAL(toggled(bool)), this, SLOT(slotCheckAllFieldsFilled()));
    connect( chooseLeader, SIGNAL(clicked()), this, SLOT(slotChooseLeader()) );

    connect(resourcesBrowseBtn, SIGNAL(clicked()), this, SLOT(openResourcesFile()));
    connect(projectsBrowseBtn, SIGNAL(clicked()), this, SLOT(openProjectsPlace()));

    connect(projectsLoadBtn, SIGNAL(clicked()), this, SLOT(loadProjects()));
    connect(projectsClearBtn, SIGNAL(clicked()), this, SLOT(clearProjects()));
}


bool MainProjectPanel::ok() {
    if (useSharedResources->isChecked() && resourcesFile->text().isEmpty()) {
        return false;
    }
    return true;
}

MacroCommand *MainProjectPanel::buildCommand() {
    MacroCommand *m = 0;
    KUndo2MagicString c = kundo2_i18n("Modify main project");
    if (project.name() != namefield->text()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new NodeModifyNameCmd(project, namefield->text()));
    }
    if (project.leader() != leaderfield->text()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new NodeModifyLeaderCmd(project, leaderfield->text()));
    }
    if (startDateTime() != project.constraintStartTime()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new ProjectModifyStartTimeCmd(project, startDateTime()));
    }
    if (endDateTime() != project.constraintEndTime()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new ProjectModifyEndTimeCmd(project, endDateTime()));
    }
    if (project.useSharedResources() != useSharedResources->isChecked()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new UseSharedResourcesCmd(&project, useSharedResources->isChecked()));
    }
    if (project.sharedResourcesFile() != resourcesFile->text()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new SharedResourcesFileCmd( &project, resourcesFile->text()));
    }
    QString place = projectsPlace->text();
    if (projectsType->currentIndex() == 0 /*dir*/ && !place.isEmpty() && !place.endsWith('/')) {
        place.append('/');
    }
    QUrl sharedProjectsUrl(place);
    if (project.sharedProjectsUrl() != sharedProjectsUrl) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new SharedProjectsUrlCmd( &project, sharedProjectsUrl));
    }
    if (project.loadProjectsAtStartup() != projectsLoadAtStartup->isChecked()) {
        if (!m) m = new MacroCommand(c);
        m->addCommand(new LoadProjectsAtStartupCmd( &project, projectsLoadAtStartup->isChecked()));
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
    bool state = !namefield->text().isEmpty();
    if (!project.isSharedResourcesLoaded()) {
        state = !useSharedResources->isChecked() || !resourcesFile->text().isEmpty();
    }
    emit obligatedFieldsFilled(state);
}


void MainProjectPanel::slotChooseLeader()
{
#ifdef PLAN_KDEPIMLIBS_FOUND
    QPointer<Akonadi::EmailAddressSelectionDialog> dlg = new Akonadi::EmailAddressSelectionDialog( this );
    if ( dlg->exec() && dlg ) {
        QStringList names;
        const Akonadi::EmailAddressSelection::List selections = dlg->selectedAddresses();
        foreach ( const Akonadi::EmailAddressSelection &selection, selections ) {
            QString s = selection.name();
            if ( ! selection.email().isEmpty() ) {
                if ( ! selection.name().isEmpty() ) {
                    s += " <";
                }
                s += selection.email();
                if ( ! selection.name().isEmpty() ) {
                    s += '>';
                }
                if ( ! s.isEmpty() ) {
                    names << s;
                }
            }
        }
        if ( ! names.isEmpty() ) {
            leaderfield->setText( names.join( ", " ) );
        }
    }
#endif
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
    debugPlan;
    startTime->setEnabled(true);
    startDate->setEnabled(true);
    endTime->setEnabled(true);
    endDate->setEnabled(true);
}


QDateTime MainProjectPanel::startDateTime()
{
    return QDateTime(startDate->date(), startTime->time(), Qt::LocalTime);
}


QDateTime MainProjectPanel::endDateTime()
{
    return QDateTime(endDate->date(), endTime->time(), Qt::LocalTime);
}

void MainProjectPanel::openResourcesFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Resources"), "", tr("Resources file (*.plan)"));
    resourcesFile->setText(fileName);
}

void MainProjectPanel::openProjectsPlace()
{
    if (projectsType->currentIndex() == 0 /*Directory*/) {
        qInfo()<<Q_FUNC_INFO<<"Directory";
        QString dirName = QFileDialog::getExistingDirectory(this, tr("Projects Directory"));
        projectsPlace->setText(dirName);
        return;
    }
    if (projectsType->currentIndex() == 1 /*File*/) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Projects"), "", tr("Projects file (*)"));
        projectsPlace->setText(fileName);
        return;
    }
    Q_ASSERT(false); // Unimplemented projects type
}

bool MainProjectPanel::loadSharedResources() const
{
    return useSharedResources->isChecked();
}

void MainProjectPanel::loadProjects()
{
    QString place = projectsPlace->text();
    if (projectsType->currentIndex() == 0 /*dir*/ && !place.isEmpty() && !place.endsWith('/')) {
        place.append('/');
    }
    QUrl url(place);
    emit loadResourceAssignments(url);
}

void MainProjectPanel::clearProjects()
{
    emit clearResourceAssignments();
}

}  //KPlato namespace
