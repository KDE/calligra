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

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>

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

    connect(accountList, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    connect(accountList, SIGNAL(itemRenamed(QListViewItem*, int)), SLOT(slotChanged()));
    
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

KCommand *KPTAccountsPanel::buildCommand(KPTPart *part) {
    //TODO
    return 0;
}


void KPTAccountsPanel::slotChanged() {
    emit changed(true);
}

void KPTAccountsPanel::slotSelectionChanged() {
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
    if (item) {
        if (item->parent()) {
            new AccountItem(item->parent(), item);
        } else {
            new AccountItem(accountList, item);
        }
    } else {
        new AccountItem(accountList);
    }
}

void KPTAccountsPanel::slotSubBtn() {
    kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = accountList->selectedItem();
    if (item) {
        new AccountItem(item);
    } else {
        new AccountItem(accountList);
    }
}

void KPTAccountsPanel::save() {
    QListViewItem *myChild = accountList->firstChild();
    for (; myChild; myChild = myChild->nextSibling()) {
        save(myChild);
    }
}

void KPTAccountsPanel::save(QListViewItem *i) {
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item->account == 0) {
        kdDebug()<<k_funcinfo<<"New account: "<<item->text(0)<<endl;
        KPTAccount *a = new KPTAccount(item->text(0), item->text(1));
        if (item->parent()) {
            static_cast<AccountItem*>(item->parent())->account->append(a);
        } else {
            m_accounts.append(a);
        }
    } else {
        if (item->text(0) != item->account->name()) {
        kdDebug()<<k_funcinfo<<"Renamed: "<<item->account->name()<<" to "<<item->text(0)<<endl;
        item->account->setName(item->text(0));
        }
        if (item->text(1) != item->account->description()) {
            kdDebug()<<k_funcinfo<<"New description: "<<item->account->description()<<" to "<<item->text(1)<<endl;
            item->account->setDescription(item->text(1));
        }
    }
    QListViewItem *myChild = item->firstChild();
    for (; myChild; myChild = myChild->nextSibling()) {
        save(myChild);
    }
}

void KPTAccountsPanel::slotOk() {
    QPtrListIterator<QListViewItem> rit = m_removedItems;
    for (;rit.current(); ++rit) {
        AccountItem *item = static_cast<AccountItem*>(rit.current());
        kdDebug()<<k_funcinfo<<"Removed item"<<endl;
        m_accounts.remove(item->account);
    }
    m_removedItems.setAutoDelete(true);
    
    save();
}

} //namespace KPlato

#include "kptaccountspanel.moc"
