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

#include "kpttaskcostpanel.h"
#include "kptaccount.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptpart.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kcommand.h>

#include <kdebug.h>

namespace KPlato
{

TaskCostPanel::TaskCostPanel(Task &task, Accounts &accounts, QWidget *p, const char *n)
    : TaskCostPanelImpl(p, n),
      m_task(task),
      m_accounts(accounts) {
      
    m_accountList << i18n("None");
    m_accountList += accounts.costElements();
    setStartValues(task);
}

void TaskCostPanel::setStartValues(Task &task) {
    runningAccount->insertStringList(m_accountList);
    if (task.runningAccount()) {
        setCurrentItem(runningAccount, task.runningAccount()->name());
    }
    
    startupCost->setText(KGlobal::locale()->formatMoney(task.startupCost()));
    startupAccount->insertStringList(m_accountList);
    if (task.startupAccount()) {
        setCurrentItem(startupAccount, task.startupAccount()->name());
    }
    
    shutdownCost->setText(KGlobal::locale()->formatMoney(task.shutdownCost()));
    shutdownAccount->insertStringList(m_accountList);
    if (task.shutdownAccount()) {
        setCurrentItem(shutdownAccount, task.shutdownAccount()->name());
    }
}

void TaskCostPanel::setCurrentItem(QComboBox *box, QString name) {
    box->setCurrentItem(0);
    for (int i = 0; i < box->count(); ++i) {
        if (name == box->text(i)) {
            box->setCurrentItem(i);
            break;
        }
    }
}

KCommand *TaskCostPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task Cost"));
    bool modified = false;
    
    if (!m_task.runningAccount() ||
        (m_task.runningAccount()->name() != runningAccount->currentText())) {
        cmd->addCommand(new NodeModifyRunningAccountCmd(part, m_task, m_accounts.findAccount(runningAccount->currentText())));
        modified = true;
    }
    if (!m_task.startupAccount() ||
        (m_task.startupAccount()->name() != startupAccount->currentText())) {
        cmd->addCommand(new NodeModifyStartupAccountCmd(part, m_task,  m_accounts.findAccount(startupAccount->currentText())));
        modified = true;
    }
    if (!m_task.shutdownAccount() ||
        (m_task.shutdownAccount()->name() != shutdownAccount->currentText())) {
        cmd->addCommand(new NodeModifyShutdownAccountCmd(part, m_task,  m_accounts.findAccount(shutdownAccount->currentText())));
        modified = true;
    }
    double money = KGlobal::locale()->readMoney(startupCost->text());
    if (money != m_task.startupCost()) {
        cmd->addCommand(new NodeModifyStartupCostCmd(part, m_task, money));
        modified = true;
    }
    money = KGlobal::locale()->readMoney(shutdownCost->text());
    if (money != m_task.shutdownCost()) {
        cmd->addCommand(new NodeModifyShutdownCostCmd(part, m_task, money));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskCostPanel::ok() {
    if (runningAccount->currentItem() == 0 ||
        m_accounts.findAccount(runningAccount->currentText()) == 0) {
        //message
        return false;
    }
    if (startupAccount->currentItem() == 0 ||
        m_accounts.findAccount(startupAccount->currentText()) == 0) {
        //message
        return false;
    }
    if (shutdownAccount->currentItem() == 0 ||
        m_accounts.findAccount(shutdownAccount->currentText()) == 0) {
        //message
        return false;
    }
    return true;
}


TaskCostPanelImpl::TaskCostPanelImpl(QWidget *p, const char *n)
    : TaskCostPanelBase(p, n)
{
    connect(runningAccount, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(startupAccount, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(shutdownAccount, SIGNAL(activated(int)), SLOT(slotChanged()));
    connect(startupCost, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
    connect(shutdownCost, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
}

void TaskCostPanelImpl::slotChanged() {
    emit changed();
}

}  //KPlato namespace

#include "kpttaskcostpanel.moc"
