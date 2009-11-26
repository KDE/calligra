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

#include "kpttaskcostpanel.h"
#include "kptaccount.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptproject.h"

#include <kmessagebox.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

TaskCostPanel::TaskCostPanel(Task &task, Accounts &accounts, QWidget *p, const char *n)
    : TaskCostPanelImpl(p, n),
      m_task(task),
      m_accounts(accounts),
      m_locale( 0 )
{
    const Project *project = qobject_cast<const Project*>( task.projectNode() );
    if ( project ) {
        m_locale = project->locale();
    }
    if ( m_locale == 0 ) {
        m_locale = KGlobal::locale();
    }
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
    
    startupCost->setText(m_locale->formatMoney(task.startupCost()));
    startupAccount->addItems(m_accountList);
    m_oldstartup = m_accounts.findStartupAccount(task);
    if (m_oldstartup) {
        setCurrentItem(startupAccount, m_oldstartup->name());
    }
    
    shutdownCost->setText(m_locale->formatMoney(task.shutdownCost()));
    shutdownAccount->addItems(m_accountList);
    m_oldshutdown = m_accounts.findShutdownAccount(task);
    if (m_oldshutdown) {
        setCurrentItem(shutdownAccount, m_oldshutdown->name());
    }
}

void TaskCostPanel::setCurrentItem(QComboBox *box, const QString& name) {
    box->setCurrentIndex(0);
    for (int i = 0; i < box->count(); ++i) {
        if (name == box->itemText(i)) {
            box->setCurrentIndex(i);
            break;
        }
    }
}

MacroCommand *TaskCostPanel::buildCommand() {
    MacroCommand *cmd = new MacroCommand(i18n("Modify Task Cost"));
    bool modified = false;
    
    if ((m_oldrunning == 0 && runningAccount->currentIndex() != 0) ||
        (m_oldrunning && m_oldrunning->name() != runningAccount->currentText())) {
        cmd->addCommand(new NodeModifyRunningAccountCmd(m_task, m_oldrunning, m_accounts.findAccount(runningAccount->currentText())));
        modified = true;
    }
    if ((m_oldstartup == 0 && startupAccount->currentIndex() != 0) ||
        (m_oldstartup && m_oldstartup->name() != startupAccount->currentText())) {
        cmd->addCommand(new NodeModifyStartupAccountCmd(m_task, m_oldstartup,  m_accounts.findAccount(startupAccount->currentText())));
        modified = true;
    }
    if ((m_oldshutdown == 0 && shutdownAccount->currentIndex() != 0) ||
        (m_oldshutdown && m_oldshutdown->name() != shutdownAccount->currentText())) {
        cmd->addCommand(new NodeModifyShutdownAccountCmd(m_task, m_oldshutdown,  m_accounts.findAccount(shutdownAccount->currentText())));
        modified = true;
    }
    double money = m_locale->readMoney(startupCost->text());
    if (money != m_task.startupCost()) {
        cmd->addCommand(new NodeModifyStartupCostCmd(m_task, money));
        modified = true;
    }
    money = m_locale->readMoney(shutdownCost->text());
    if (money != m_task.shutdownCost()) {
        cmd->addCommand(new NodeModifyShutdownCostCmd(m_task, money));
        modified = true;
    }
    if (!modified) {
        delete cmd;
        return 0;
    }
    return cmd;
}

bool TaskCostPanel::ok() {
    if (runningAccount->currentIndex() == 0 ||
        m_accounts.findAccount(runningAccount->currentText()) == 0) {
        //message
        return false;
    }
    if (startupAccount->currentIndex() == 0 ||
        m_accounts.findAccount(startupAccount->currentText()) == 0) {
        //message
        return false;
    }
    if (shutdownAccount->currentIndex() == 0 ||
        m_accounts.findAccount(shutdownAccount->currentText()) == 0) {
        //message
        return false;
    }
    return true;
}


TaskCostPanelImpl::TaskCostPanelImpl(QWidget *p, const char *n)
    : QWidget(p)
{
    setObjectName(n);
    setupUi(this);
    
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
