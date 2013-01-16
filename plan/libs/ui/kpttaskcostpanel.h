/* This file is part of the KDE project
   Copyright (C) 2005-2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

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

#include "kplatoui_export.h"

#include "ui_kpttaskcostpanelbase.h"

class KLocale;

namespace KPlato
{

class TaskCostPanel;
class Account;
class Accounts;
class Task;
class MacroCommand;

class TaskCostPanelImpl : public QWidget, public Ui_TaskCostPanelBase {
    Q_OBJECT
public:
    explicit TaskCostPanelImpl(QWidget *parent=0, const char *name=0);

signals:
    void changed();

public slots:
    void slotChanged();
};

class TaskCostPanel : public TaskCostPanelImpl {
    Q_OBJECT
public:
    TaskCostPanel(Task &task, Accounts &accounts, QWidget *parent=0, const char *name=0);

    MacroCommand *buildCommand();

    bool ok();

    void setStartValues(Task &task);

protected:
    void setCurrentItem(QComboBox *box, const QString& name);
    
private:
    Task &m_task;
    Accounts &m_accounts;
    QStringList m_accountList;
    Account *m_oldrunning;
    Account *m_oldstartup;
    Account *m_oldshutdown;
    const KLocale *m_locale;
};

} //KPlato namespace

#endif // TASKCOSTPANEL_H
