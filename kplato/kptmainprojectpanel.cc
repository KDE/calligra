/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptmainprojectpanel.h"

#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <klineedit.h>
#include <ktextedit.h>
#include <kdatetimewidget.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcommand.h>

#include <kdebug.h>

#include "kptproject.h"
#include "kptcommand.h"

namespace KPlato
{

KPTMainProjectPanel::KPTMainProjectPanel(KPTProject &p, QWidget *parent, const char *name)
    : KPTMainProjectPanelBase(parent, name),
      project(p)
{
    namefield->setText(project.name());
    idfield->setText(project.id());
    leaderfield->setText(project.leader());
    descriptionfield->setText(project.description());

    startDateTime->setDateTime(project.startTime());
    endDateTime->setDateTime(project.endTime());
    if (project.constraint() == KPTNode::MustStartOn) {
        schedulingGroup->setButton(0);
        startDateTime->setEnabled(true);
    }
    else if (project.constraint() == KPTNode::MustFinishOn) {
        schedulingGroup->setButton(1);
        endDateTime->setEnabled(true);
    }
    namefield->setFocus();
}


bool KPTMainProjectPanel::ok() {
    if (idfield->text() != project.id() && KPTNode::find(idfield->text())) {
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
    if (bStartDate->state() && project.constraint() != KPTNode::MustStartOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyConstraintCmd(part, project, KPTNode::MustStartOn));
    } 
    if (bEndDate->state() && project.constraint() != KPTNode::MustFinishOn) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyConstraintCmd(part, project, KPTNode::MustFinishOn));
    } 
    if (startDateTime->dateTime() != project.startTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyStartTimeCmd(part, project, startDateTime->dateTime()));
    }
    if (endDateTime->dateTime() != project.endTime()) {
        if (!m) m = new KMacroCommand(c);
        m->addCommand(new KPTNodeModifyEndTimeCmd(part, project, endDateTime->dateTime()));
    }
    return m;
}


}  //KPlato namespace

#include "kptmainprojectpanel.moc"
