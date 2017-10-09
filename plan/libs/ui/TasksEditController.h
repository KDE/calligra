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

#ifndef TASKSEDITCONTROLLER_H
#define TASKSEDITCONTROLLER_H

#include "kplatoui_export.h"

#include <QObject>

class KUndo2Command;

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

class TasksEditDialog;

/**
 * The dialog that allows you to alter multiple tasks.
 */
class KPLATOUI_EXPORT TasksEditController : public QObject
{
    Q_OBJECT
public:
    TasksEditController(Project &project, const QList<Task*> &tasks, QObject *parent = 0);
    ~TasksEditController();

public Q_SLOTS:
    void activate();

Q_SIGNALS:
    void addCommand(KUndo2Command *cmd);

private Q_SLOTS:
    void finish(int result);

private:
    Project &m_project;
    const QList<Task*> m_tasks;
    TasksEditDialog *m_dlg;
};


} //KPlato namespace

#endif // TASKSEDITCONTROLLER_H
