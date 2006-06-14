/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@sonofthor.dk
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include <kpagedialog.h>

class Duration;

class KLineEdit;
class KCommand;
class KTextEdit;
class KComboBox;
class KDoubleNumInput;

class Q3DateTimeEdit;
class QSpinBox;
class Q3ButtonGroup;
class Q3ListBox;
class Q3Table;
class QDateTime;

//TODO ui files are not in the KPlato namespace!!

namespace KPlato
{

class Accounts;
class TaskGeneralPanel;
class RequestResourcesPanel;
class TaskCostPanel;
class Part;
class Task;
class StandardWorktime;

/**
 * The dialog that shows and allows you to alter any task.
 */
class TaskDialog : public KPageDialog {
    Q_OBJECT
public:
    /**
     * The constructor for the task settings dialog.
     * @param task the task to show
     * @param accounts all defined accounts
     * @param workTime defines the number of hours pr day and week
     * @param baseline if true, project is baselined
     * @param parent parent widget
     */
    TaskDialog(Task &task, Accounts &accounts, StandardWorktime *workTime=0, bool baseline=false,  QWidget *parent=0);

    KCommand *buildCommand(Part *part);

protected slots:
    void slotOk();

private:
    TaskGeneralPanel *m_generalTab;
    RequestResourcesPanel *m_resourcesTab;
    TaskCostPanel *m_costTab;
};

} //KPlato namespace

#endif // TASKDIALOG_H
