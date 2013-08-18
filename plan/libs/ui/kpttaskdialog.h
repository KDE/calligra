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

#ifndef KPTTASKDIALOG_H
#define KPTTASKDIALOG_H

#include "kplatoui_export.h"

#include <kopagedialog.h>


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
class StandardWorktime;
class MacroCommand;
class RequestResourcesPanel;

/**
 * The dialog that shows and allows you to alter any task.
 */
class KPLATOUI_EXPORT TaskDialog : public KoPageDialog {
    Q_OBJECT
public:
    /**
     * The constructor for the task settings dialog.
     * @param task the task to show
     * @param accounts all defined accounts
     * @param parent parent widget
     */
    TaskDialog(Project &project, Task &task, Accounts &accounts, QWidget *parent=0);

    virtual MacroCommand *buildCommand();

protected slots:
    void slotButtonClicked(int button);
    void slotTaskRemoved( Node *node );
    void slotCurrentChanged( KPageWidgetItem*, KPageWidgetItem* );

protected:
    Project &m_project;
    Node *m_node;

    TaskGeneralPanel *m_generalTab;
    RequestResourcesPanel *m_resourcesTab;
    DocumentsPanel *m_documentsTab;
    TaskCostPanel *m_costTab;
    TaskDescriptionPanel *m_descriptionTab;
};

class KPLATOUI_EXPORT TaskAddDialog : public TaskDialog {
    Q_OBJECT
public:
    TaskAddDialog(Project &project, Task &task, Node *currentNode, Accounts &accounts, QWidget *parent=0);
    ~TaskAddDialog();

    virtual MacroCommand *buildCommand();

protected slots:
    void slotNodeRemoved( Node* );

private:
    Node *m_currentnode;
};

class KPLATOUI_EXPORT SubTaskAddDialog : public TaskDialog {
    Q_OBJECT
public:
    SubTaskAddDialog(Project &project, Task &task, Node *currentNode, Accounts &accounts, QWidget *parent=0);
    ~SubTaskAddDialog();

    virtual MacroCommand *buildCommand();

protected slots:
    void slotNodeRemoved( Node* );

private:
    Node *m_currentnode;
};

} //KPlato namespace

#endif // TASKDIALOG_H
