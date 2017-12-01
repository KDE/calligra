/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 -2010 Dag Andersen <danders@get2net.dk>

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

#include "TasksEditController.h"
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

TasksEditController::TasksEditController(Project &project, const QList<Task*> &tasks, QObject *p)
    : QObject(p)
    , m_project(project)
    , m_tasks(tasks)
    , m_dlg(0)
{
}

TasksEditController::~TasksEditController()
{
    delete m_dlg;
}

void TasksEditController::activate()
{
    m_dlg = new TasksEditDialog(m_project, m_tasks);
    connect(m_dlg, SIGNAL(finished(int)), this, SLOT(finish(int)));
    m_dlg->show();
    m_dlg->raise();
    m_dlg->activateWindow();
}

void TasksEditController::finish(int result)
{
    if (!m_dlg || sender() != m_dlg) {
        return;
    }
    if (result == QDialog::Accepted) {
        MacroCommand *m = m_dlg->buildCommand();
        if (m) {
            emit addCommand(m);
        }
    }
    m_dlg->hide();
    deleteLater();
}

}  //KPlato namespace
