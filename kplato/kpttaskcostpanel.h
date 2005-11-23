/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library Cost Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library Cost Public License for more details.

   You should have received a copy of the GNU Library Cost Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTTASKCOSTPANEL_H
#define KPTTASKCOSTPANEL_H

#include "kpttaskcostpanelbase.h"

class KCommand;

namespace KPlato
{

class TaskCostPanel;
class Account;
class Accounts;
class Part;
class Task;

class TaskCostPanelImpl : public TaskCostPanelBase {
    Q_OBJECT
public:
    TaskCostPanelImpl(QWidget *parent=0, const char *name=0);

signals:
    void changed();

public slots:
    void slotChanged();
};

class TaskCostPanel : public TaskCostPanelImpl {
    Q_OBJECT
public:
    TaskCostPanel(Task &task, Accounts &accounts, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(Part *part);

    bool ok();

    void setStartValues(Task &task);

protected:
    void setCurrentItem(QComboBox *box, QString name);
    
private:
    Task &m_task;
    Accounts &m_accounts;
    QStringList m_accountList;
    Account *m_oldrunning;
    Account *m_oldstartup;
    Account *m_oldshutdown;
};

} //KPlato namespace

#endif // TASKCOSTPANEL_H
