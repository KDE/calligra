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
    runningAccount->addItems(m_accountList);
    m_oldrunning = m_accounts.findRunningAccount(task);
    if (m_oldrunning) {
        setCurrentItem(runningAccount, m_oldrunning->name());
    }
    
    startupCost->setText(KGlobal::locale()->formatMoney(task.startupCost()));
    startupAccount->addItems(m_accountList);
    m_oldstartup = m_accounts.findStartupAccount(task);
    if (m_oldstartup) {
        setCurrentItem(startupAccount, m_oldstartup->name());
    }
    
    shutdownCost->setText(KGlobal::locale()->formatMoney(task.shutdownCost()));
    shutdownAccount->addItems(m_accountList);
    m_oldshutdown = m_accounts.findShutdownAccount(task);
    if (m_oldshutdown) {
        setCurrentItem(shutdownAccount, m_oldshutdown->name());
    }
}

void TaskCostPanel::setCurrentItem(QComboBox *box, QString name) {
    box->setCurrentIndex(0);
    for (int i = 0; i < box->count(); ++i) {
        if (name == box->text(i)) {
            box->setCurrentIndex(i);
            break;
        }
    }
}

KCommand *TaskCostPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Task Cost"));
    bool modified = false;
    
    if ((m_oldrunning == 0 && runningAccount->currentItem() != 0) ||
        (m_oldrunning && m_oldrunning->name() != runningAccount->currentText())) {
        cmd->addCommand(new NodeModifyRunningAccountCmd(part, m_task, m_oldrunning, m_accounts.findAccount(runningAccount->currentText())));
        modified = true;
    }
    if ((m_oldstartup == 0 && startupAccount->currentItem() != 0) ||
        (m_oldstartup && m_oldstartup->name() != startupAccount->currentText())) {
        cmd->addCommand(new NodeModifyStartupAccountCmd(part, m_task, m_oldstartup,  m_accounts.findAccount(startupAccount->currentText())));
        modified = true;
    }
    if ((m_oldshutdown == 0 && shutdownAccount->currentItem() != 0) ||
        (m_oldshutdown && m_oldshutdown->name() != shutdownAccount->currentText())) {
        cmd->addCommand(new NodeModifyShutdownAccountCmd(part, m_task, m_oldshutdown,  m_accounts.findAccount(shutdownAccount->currentText())));
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
