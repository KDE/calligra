/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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

#include "TasksEditDialog.h"
#include "kpttaskcostpanel.h"
#include "kpttaskgeneralpanel.h"
#include "kptrequestresourcespanel.h"
#include "kptdocumentspanel.h"
#include "kpttaskdescriptiondialog.h"
#include "kptcommand.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptproject.h"

#include <KoVBox.h>

#include <KLocalizedString>

namespace KPlato
{

TasksEditDialog::TasksEditDialog(Project &project, const QList<Task*> &tasks, QWidget *p)
    : KPageDialog(p)
    , m_project(project)
    , m_tasks(tasks)
{
    m_task = new Task(project.taskDefaults());

    setWindowTitle(i18n("Tasks Settings"));
    setFaceType(KPageDialog::Tabbed);

    KoVBox *page;

    // Create all the tabs.
    page =  new KoVBox();
    addPage(page, i18n("&Resources"));
    m_resourcesTab = new RequestResourcesPanel(page, project, *m_task);

    resize(size().expandedTo(QSize(200, 75)));
}

void TasksEditDialog::setButtonOkEnabled(bool enabled)
{
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(enabled);
}

void TasksEditDialog::slotCurrentChanged(KPageWidgetItem *current, KPageWidgetItem */*prev*/)
{
}

void TasksEditDialog::slotTaskRemoved(Node *node)
{
    if (node->type() == Node::Type_Task && m_tasks.contains(static_cast<Task*>(node))) {
        reject();
    }
}

MacroCommand *TasksEditDialog::buildCommand()
{
    MacroCommand *m = new MacroCommand(kundo2_i18n("Modify tasks"));
    bool modified = false;

    for (Task *t : m_tasks) {
        MacroCommand *c = m_resourcesTab->buildCommand(t);
        if (c) {
            m->addCommand(c);
            modified = true;
        }
    }
    if (!modified) {
        delete m;
        m = 0;
    }
    return m;
}

void TasksEditDialog::accept()
{
    KPageDialog::accept();
}

}  //KPlato namespace
