/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>

#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

class AccountItem : public QListViewItem {
public:
    AccountItem(QListView *parent)
    : QListViewItem(parent), account(0)
    { init(); }
    AccountItem(QListViewItem *parent)
    : QListViewItem(parent), account(0)
    { init(); }
    AccountItem(QListView *parent, QString label1, QString label2 = QString::null)
    : QListViewItem(parent, label1, label2), account(0)
    { init(); }
    AccountItem(QListViewItem *parent, QString label1, QString label2 = QString::null)
    : QListViewItem(parent, label1, label2), account(0)
    { init(); }
    AccountItem(QListView *parent, QListViewItem *after)
    : QListViewItem(parent, after), account(0)
    { init(); }
    AccountItem(QListViewItem *parent, QListViewItem *after)
    : QListViewItem(parent, after), account(0)
    { init(); }
    
    KPTAccount *account;
private:
    void init() {
        setRenameEnabled(0, true);  
        setRenameEnabled(1, true);
        setOpen(true);
    
    }
};

KPTAccountsPanel::KPTAccountsPanel(KPTAccounts &acc, QWidget *p, const char *n)
    : KPTAccountsPanelBase(p, n),
      m_accounts(acc)
{

    accountList->setRootIsDecorated(true);
    addItems(accountList, acc);

    slotSelectionChanged();
    
    connect(accountList, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    connect(accountList, SIGNAL(itemRenamed(QListViewItem*, int)), SLOT(slotItemRenamed(QListViewItem*, int)));
    
    connect(removeBtn, SIGNAL(clicked()), SLOT(slotRemoveBtn()));
    connect(newBtn, SIGNAL(clicked()), SLOT(slotNewBtn()));
    connect(subBtn, SIGNAL(clicked()), SLOT(slotSubBtn()));
}
    
void KPTAccountsPanel::addItems(QListView *lv, KPTAccounts &acc) {
    kdDebug()<<k_funcinfo<<"No of accs: "<<acc.accountList().count()<<endl;
    KPTAccountListIterator it = acc.accountList();
    for (; it.current(); ++it) {
        QString n = it.current()->name();
        QString d = it.current()->description();
        AccountItem *item = new AccountItem(lv, n, d);
        item->account = it.current();
        addItems(item, it.current());
    }
}

void KPTAccountsPanel::addItems(QListViewItem *item, KPTAccount *acc) {
    KPTAccountListIterator it = acc->accountList();
    for (; it.current(); ++it) {
        QString n = it.current()->name();
        QString d = it.current()->description();
        AccountItem *ai = new AccountItem(item, n, d);
        ai->account = it.current();
        addItems(ai, it.current());
    }
}

void KPTAccountsPanel::slotChanged() {
    emit changed(true);
}

void KPTAccountsPanel::slotSelectionChanged() {
    kdDebug()<<k_funcinfo<<endl;
    QListViewItem *i = accountList->selectedItem();
    removeBtn->setEnabled((bool)i);
    newBtn->setEnabled((bool)i);
    subBtn->setEnabled((bool)i);
}

void KPTAccountsPanel::slotItemRenamed(QListViewItem *item, int col) {
    if (col != 0) {
        slotChanged();
        return;
    }
    QString text = item->text(0);
    QListViewItemIterator it(item->listView());
    for (; it.current(); ++it) {
        if (it.current() != item && it.current()->text(0) == text) {
            item->startRename(0);
            break;
        }
    }
    slotChanged();
}

void KPTAccountsPanel::slotRemoveBtn() {
    QListViewItem *item = accountList->selectedItem();
    if (item == 0)
        return;
    if (item->parent()) {
        item->parent()->takeItem(item);
        m_removedItems.append(item);
    } else {
        accountList->takeItem(item);
        m_removedItems.append(item);
    }
    slotChanged();
}

void KPTAccountsPanel::slotNewBtn() {
    kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = accountList->selectedItem();
    if (item && item->text(0).isEmpty()) {
        return;
    }
    QListViewItem *n;
    if (item) {
        if (item->parent()) {
            n = new AccountItem(item->parent(), item);
        } else {
            n = new AccountItem(accountList, item);
        }
    } else {
        n = new AccountItem(accountList);
    }
    n->startRename(0);
}

void KPTAccountsPanel::slotSubBtn() {
    kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = accountList->selectedItem();
    if (item && item->text(0).isEmpty()) {
        return;
    }
    QListViewItem *n;
    if (item) {
        n = new AccountItem(item);
    } else {
        n = new AccountItem(accountList);
    }
    n->startRename(0);
}

KCommand *KPTAccountsPanel::buildCommand(KPTPart *part) {
    KMacroCommand *cmd = 0;
    // First remove
    QPtrListIterator<QListViewItem> rit = m_removedItems;
    for (;rit.current(); ++rit) {
        AccountItem *item = static_cast<AccountItem*>(rit.current());
        kdDebug()<<k_funcinfo<<"Removed item"<<endl;
        if (!cmd) cmd = new KMacroCommand(i18n("Modify accounts"));
        cmd->addCommand(new KPTRemoveAccountCmd(part, part->getProject(), item->account));
    }
    m_removedItems.setAutoDelete(true);
    // Then add/modify
    KCommand *c = save(part, part->getProject());
    if (c) {
        if (!cmd) cmd = new KMacroCommand(i18n("Modify accounts"));
        cmd->addCommand(c);
    }
    return cmd;
}

KCommand *KPTAccountsPanel::save(KPTPart *part, KPTProject &project) {
    KMacroCommand *cmd=0;
    QListViewItem *myChild = accountList->firstChild();
    for (; myChild; myChild = myChild->nextSibling()) {
        KCommand *c = save(part, project, myChild);
        if (c) {
            if (!cmd) cmd = new KMacroCommand("");
            cmd->addCommand(c);
        }
    }
    return cmd;
}

KCommand *KPTAccountsPanel::save(KPTPart *part, KPTProject &project, QListViewItem *i) {
    KMacroCommand *cmd=0;
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item->account == 0) {
        kdDebug()<<k_funcinfo<<"New account: "<<item->text(0)<<endl;
        if (!cmd) cmd = new KMacroCommand("");
        KPTAccount *a = new KPTAccount(item->text(0), item->text(1));
        if (item->parent()) {
            kdDebug()<<k_funcinfo<<"New account: "<<item->text(0)<<endl;
            cmd->addCommand(new KPTAddAccountCmd(part, project, a, item->parent()->text(0)));
        } else {
            cmd->addCommand(new KPTAddAccountCmd(part, project, a));
        }
    } else {
        if (item->text(0) != item->account->name()) {
            if (!cmd) cmd = new KMacroCommand("");
            kdDebug()<<k_funcinfo<<"Renamed: "<<item->account->name()<<" to "<<item->text(0)<<endl;
            cmd->addCommand(new KPTRenameAccountCmd(part, item->account, item->text(0)));
        }
        if (item->text(1) != item->account->description()) {
            if (!cmd) cmd = new KMacroCommand("");
            kdDebug()<<k_funcinfo<<"New description: "<<item->account->description()<<" to "<<item->text(1)<<endl;
            cmd->addCommand(new KPTModifyAccountDescriptionCmd(part, item->account, item->text(1)));
        }
    }
    QListViewItem *myChild = item->firstChild();
    for (; myChild; myChild = myChild->nextSibling()) {
        KCommand *c = save(part, project, myChild);
        if (c) {
            if (!cmd) cmd = new KMacroCommand("");
            cmd->addCommand(c);
        }
    }
    return cmd;
}

void KPTAccountsPanel::slotOk() {
}

} //namespace KPlato

#include "kptaccountspanel.moc"
