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
   Boston, MA 02110-1301, USA.
*/

#ifndef TASKSEDITDIALOGWIDGET_H
#define TASKSEDITDIALOGWIDGET_H

#include "kplatoui_export.h"

#include <kpagedialog.h>


namespace KPlato
{

class Accounts;
class TaskGeneralPanel;
class RequestResourcesPanel;
class DocumentsPanel;
class TaskCostPanel;
class TaskDescriptionPanel;
class Node;
class Task;
class Project;
class MacroCommand;

/**
 * The dialog that shows and allows you to alter any task.
 */
class KPLATOUI_EXPORT TasksEditDialog : public KPageDialog {
    Q_OBJECT
public:
    /**
     * The constructor for the tasks settings dialog.
     * @param tasks the list of tasks to be edited
     * @param parent parent widget
     */
    TasksEditDialog(Project &project, const QList<Task*> &tasks, QWidget *parent=0);

    virtual MacroCommand *buildCommand();

protected Q_SLOTS:
    void accept();
    void setButtonOkEnabled(bool enabled);

    void slotTaskRemoved( Node *node );
    void slotCurrentChanged( KPageWidgetItem*, KPageWidgetItem* );

protected:
    Project &m_project;
    const QList<Task*> m_tasks;
    Task *m_task;

    RequestResourcesPanel *m_resourcesTab;
};

} //KPlato namespace

#endif // TASKSEDITDIALOGWIDGET_H
