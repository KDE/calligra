/* This file is part of the KDE project
   Copyright (C) 2005-2007 Dag Andersen <danders@get2net.dk>

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

#include "kptaccountspanel.h"
#include "kptaccount.h"
#include "kptcommand.h"
#include "kptproject.h"

#include <QTreeWidget>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QHeaderView>

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

class AccountItem : public QTreeWidgetItem {
public:
    AccountItem(AccountsPanel &pan, QTreeWidget *parent)
    : QTreeWidgetItem(parent), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QTreeWidget *parent, const QString& label1, const QString& label2 = QString())
    : QTreeWidgetItem(parent), account(0), panel(pan) { 
        setText(0, label1);
        setText(1, label2);
        init(); 
    }
    AccountItem(AccountsPanel &pan, QTreeWidgetItem *parent, const QString& label1, const QString& label2 = QString())
    : QTreeWidgetItem(parent), account(0), panel(pan) { 
        setText(0, label1);
        setText(1, label2);
        init(); 
    }
    AccountItem(AccountsPanel &pan, QTreeWidget *parent, QTreeWidgetItem *after)
    : QTreeWidgetItem(parent, after), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QTreeWidgetItem *parent, QTreeWidgetItem *after)
    : QTreeWidgetItem(parent, after), account(0), panel(pan)
    { init(); }

    Account *account;
    bool isDefault;
    AccountsPanel &panel;

private:
    void init() {
        isDefault = false;
        setFlags(flags() | Qt::ItemIsEditable);
    }
};

AccountsPanel::AccountsPanel(Project &project, Accounts &acc, QWidget *p)
    : AccountsPanelBase(p),
      m_project(project),
      m_accounts(acc),
      m_currentIndex(0),
      m_renameItem(0)
{
    accountList->setHeaderLabels(QStringList()<<"Account"<<"Description");
    accountList->header()->setStretchLastSection(true);
    addItems(accountList, acc);

    slotSelectionChanged();
    
    connect(accountList, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));
    
    connect(removeBtn, SIGNAL(clicked()), SLOT(slotRemoveBtn()));
    connect(newBtn, SIGNAL(clicked()), SLOT(slotNewBtn()));
    connect(subBtn, SIGNAL(clicked()), SLOT(slotSubBtn()));

    connect(accountsComboBox, SIGNAL(activated(int)), SLOT(slotActivated(int)));
    
    connect(accountList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
}
    
void AccountsPanel::addItems(QTreeWidget *lv, Accounts &acc) {
    //kDebug()<<"No of accs:"<<acc.accountList().count();
    AccountListIterator it = acc.accountList();
    foreach (Account *a, acc.accountList()) {
        QString n = a->name();
        QString d = a->description();
        AccountItem *item = new AccountItem(*this, lv, n, d);
        item->account = a;
        item->isDefault = (a == acc.defaultAccount());
        if (a->isElement()) {
            addElement(item);
        }
        addItems(item, a);
    }
}

void AccountsPanel::addItems(QTreeWidgetItem *item, Account *acc) {
    foreach (Account *a, acc->accountList()) {
        QString n = a->name();
        QString d = a->description();
        AccountItem *ai = new AccountItem(*this, item, n, d);
        ai->account = a;
        ai->isDefault = (a == acc->list()->defaultAccount());
        if (a->isElement()) {
            addElement(ai);
        }
        addItems(ai, a);
    }
}

void AccountsPanel::addElement(QTreeWidgetItem *item) {
    if (item->parent()) {
        removeElement(item->parent());
    }
    m_elements.insert(item->text(0), item);
    //kDebug()<<item->text(0);
    refreshDefaultAccount();
}

void AccountsPanel::removeElement(const QString& key) {
    QHash<QString, QTreeWidgetItem*>::Iterator it = m_elements.find(key);
    for (it = m_elements.begin(); it != m_elements.end(); ++it) {
        removeElement(*it);
    }
}

void AccountsPanel::removeElement(QTreeWidgetItem *item) {
    static_cast<AccountItem*>(item)->isDefault = false;
    m_elements.remove(item->text(0));
    refreshDefaultAccount();
}

void AccountsPanel::refreshDefaultAccount() {
    accountsComboBox->clear();
    m_currentIndex = 0;
    accountsComboBox->addItem(i18n("None"));
    QStringList keylist = m_elements.uniqueKeys();
    int i=1;
    foreach (QString key, keylist) {
        accountsComboBox->addItem(key);
        if (static_cast<AccountItem*>(m_elements[key])->isDefault) {
            m_currentIndex = i;
            accountsComboBox->setCurrentIndex(i);
            //kDebug()<<"Default="<<key;
        }
        ++i;
    }
    //kDebug()<<"size="<<accountsComboBox->count();
}

void AccountsPanel::slotItemChanged(QTreeWidgetItem*, int ) {
    //kDebug()<<item->text(0)<<","<<col;
    emit changed(true);
}

void AccountsPanel::slotActivated(int index) {
    //kDebug()<<index;
    if (m_currentIndex >= (int)m_elements.count()) {
        kError()<<"currentIndex ("<<m_currentIndex<<") out of range ("<<m_elements.count()<<")"<<endl;
    } else if (m_currentIndex > 0) {
        AccountItem *i = static_cast<AccountItem*>(m_elements[accountsComboBox->text(m_currentIndex)]);
        if (i) 
            i->isDefault = false;
    }
    m_currentIndex = 0;
    if (index < (int)m_elements.size()) {
        AccountItem *i = static_cast<AccountItem*>(m_elements[accountsComboBox->currentText()]);
        if (i) {
            i->isDefault = true;
            m_currentIndex = index;
            //kDebug()<<"currentIndex="<<m_currentIndex<<","<<m_elements[accountsComboBox->currentText()]->text(0);
        }
    }
    slotChanged();
}

void AccountsPanel::slotChanged() {
    emit changed(true);
}

void AccountsPanel::slotSelectionChanged() {
    //kDebug();
    if (accountList->topLevelItemCount() == 0) {
        removeBtn->setEnabled(false);
        newBtn->setEnabled(true);
        subBtn->setEnabled(false);
        return;
    }
    bool i = accountList->selectedItems().count() > 0;
    removeBtn->setEnabled((bool)i);
    newBtn->setEnabled(true);
    subBtn->setEnabled((bool)i);
}

bool AccountsPanel::isUnique(QTreeWidgetItem *item) {
/*    QTreeWidgetItemIterator it(accountList);
    for (; it.current(); ++it) {
        if (it.current() != item && it.current()->text(0) == item->text(0)) {
            return false;
        }
    }
    return true;*/
    return false;
}

void AccountsPanel::slotRemoveBtn() {
    QList<QTreeWidgetItem*> lst = accountList->selectedItems();
    foreach (QTreeWidgetItem* i, lst) {
        slotRemoveItem(i);
    }
    slotChanged();
}

void AccountsPanel::slotNewBtn() {
    //kDebug();
    QList<QTreeWidgetItem*> lst = accountList->selectedItems();
    QTreeWidgetItem *item = 0;
    if (lst.count() > 0) {
        item = lst[0];
    }
    QTreeWidgetItem *n;
    if (item) {
        if (item->parent()) {
            n = new AccountItem(*this, item->parent(), item);
        } else {
            n = new AccountItem(*this, accountList, item);
        }
    } else {
        n = new AccountItem(*this, accountList);
    }
    accountList->clearSelection();
    n->setSelected(true);
    accountList->editItem(n);
}

void AccountsPanel::slotSubBtn() {
    //kDebug();
    QList<QTreeWidgetItem*> lst = accountList->selectedItems();
    QTreeWidgetItem* item = 0;
    if (lst.count() > 0) {
        item = lst[0];
    }
    QTreeWidgetItem *n;
    if (item) {
        n = new AccountItem(*this, item);
        item->setExpanded(true);
    } else {
        n = new AccountItem(*this, accountList);
    }
    accountList->clearSelection();
    n->setSelected(true);
    accountList->editItem(n);
}

MacroCommand *AccountsPanel::buildCommand() {
    MacroCommand *cmd = 0;
    // First remove
    while (!m_removedItems.isEmpty()) {
        AccountItem *item = static_cast<AccountItem*>(m_removedItems.takeFirst());
        //kDebug()<<"Removed item";
        if (!cmd) cmd = new MacroCommand(i18n("Modify Accounts"));
        cmd->addCommand(new RemoveAccountCmd(m_project, item->account));
        delete item;
    }
    // Then add/modify
    MacroCommand *c = save(m_project);
    if (c) {
        if (!cmd) cmd = new MacroCommand(i18n("Modify Accounts"));
        cmd->addCommand(c);
    }
    return cmd;
}

MacroCommand *AccountsPanel::save(Project &project) {
    MacroCommand *cmd=0;
    int cnt = accountList->topLevelItemCount();
    for (int i=0; i < cnt; ++i) {
        MacroCommand *c = save(project, accountList->topLevelItem(i));
        if (c) {
            if (!cmd) cmd = new MacroCommand("");
            cmd->addCommand(c);
        }
    }
    return cmd;
}

MacroCommand *AccountsPanel::save(Project &project, QTreeWidgetItem *i) {
    MacroCommand *cmd=0;
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item->account == 0) {
        if (!item->text(0).isEmpty()) {
            kDebug()<<"New account:"<<item->text(0);
            if (!cmd) cmd = new MacroCommand("");
            item->account = new Account(item->text(0), item->text(1));
            if (item->parent()) {
                kDebug()<<"New account:"<<item->text(0);
                cmd->addCommand(new AddAccountCmd(project, item->account, item->parent()->text(0)));
            } else {
                cmd->addCommand(new AddAccountCmd(project, item->account));
            }
        }
    } else {
        if (!item->text(0).isEmpty() && (item->text(0) != item->account->name())) {
            if (!cmd) cmd = new MacroCommand("");
            //kDebug()<<"Renamed:"<<item->account->name()<<" to"<<item->text(0);
            cmd->addCommand(new RenameAccountCmd(item->account, item->text(0)));
        }
        if (item->text(1) != item->account->description()) {
            if (!cmd) cmd = new MacroCommand("");
            //kDebug()<<"New description:"<<item->account->description()<<" to"<<item->text(1);
            cmd->addCommand(new ModifyAccountDescriptionCmd(item->account, item->text(1)));
        }
    }
    int cnt = item->childCount();
    for (int i=0; i < cnt; ++i) {
        QTreeWidgetItem *myChild = item->child(i);
        MacroCommand *c = save(project, myChild);
        if (c) {
            if (!cmd) cmd = new MacroCommand("");
            cmd->addCommand(c);
        }
    }
    AccountItem *ai = static_cast<AccountItem*>(m_elements[accountsComboBox->currentText()]);
    Account *newDefaultAccount = 0;
    if (ai) {
        newDefaultAccount = ai->account;
    }
    if (m_oldDefaultAccount != newDefaultAccount) {
        if (!cmd) cmd = new MacroCommand("");
        cmd->addCommand(new ModifyDefaultAccountCmd(m_accounts, m_oldDefaultAccount, newDefaultAccount));
    }
    return cmd;
}

void AccountsPanel::slotRemoveItem(QTreeWidgetItem *i) {
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item == 0)
        return;
    //kDebug()<<item->text(0);
    removeElement(item);
    QTreeWidgetItem *p = item->parent();
    if (p) {
        p->takeChild(p->indexOfChild(item));
        if (item->account) {
            m_removedItems.append(item);
        } else {
            delete item;
        }
        if (p->childCount() == 0) {
            addElement(p);
        }
    } else {
        accountList->takeTopLevelItem(accountList->indexOfTopLevelItem(item));
        if (item->account) {
            m_removedItems.append(item);
        } else {
            delete item;
        }
    }
}

void AccountsPanel::slotOk() {
	//emit slotAccountsOk;
}

} //namespace KPlato

#include "kptaccountspanel.moc"
