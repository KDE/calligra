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

#include <qcombobox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>

#include <klistview.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

class AccountItem : public KListViewItem {
public:
    AccountItem(AccountsPanel &pan, QListView *parent)
    : KListViewItem(parent), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QListViewItem *parent)
    : KListViewItem(parent), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QListView *parent, QString label1, QString label2 = QString::null)
    : KListViewItem(parent, label1, label2), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QListViewItem *parent, QString label1, QString label2 = QString::null)
    : KListViewItem(parent, label1, label2), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QListView *parent, QListViewItem *after)
    : KListViewItem(parent, after), account(0), panel(pan)
    { init(); }
    AccountItem(AccountsPanel &pan, QListViewItem *parent, QListViewItem *after)
    : KListViewItem(parent, after), account(0), panel(pan)
    { init(); }

    Account *account;
    bool isDefault;

    QString oldText;
    AccountsPanel &panel;
protected:
    virtual void cancelRename(int col) {
        //kdDebug()<<k_funcinfo<<endl;
        if ((col == 0 && oldText.isEmpty()) ||
            (!panel.isUnique(this))) {
            return;
        }
        panel.renameStopped(this);
        QListViewItem::cancelRename(col);
        setRenameEnabled(col, false);
    }
private:
    void init() {
        setRenameEnabled(0, false);  
        setRenameEnabled(1, false);
        setOpen(true);
        isDefault = false;
    
    }
};

AccountsPanel::AccountsPanel(Accounts &acc, QWidget *p, const char *n)
    : AccountsPanelBase(p, n),
      m_accounts(acc),
      m_currentIndex(0),
      m_renameItem(0)
{

    accountList->setRootIsDecorated(true);
    accountList->header()->setStretchEnabled(true, 1);
    accountList->setItemMargin(2);
    accountList->setDefaultRenameAction(QListView::Accept);
    addItems(accountList, acc);

    slotSelectionChanged();
    
    connect(accountList, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    connect(accountList, SIGNAL(itemRenamed(QListViewItem*, int)), SLOT(slotItemRenamed(QListViewItem*, int)));
    connect(accountList, SIGNAL(doubleClicked(QListViewItem*, const QPoint &, int)), SLOT(slotListDoubleClicked(QListViewItem*, const QPoint &, int)));
    
    connect(removeBtn, SIGNAL(clicked()), SLOT(slotRemoveBtn()));
    connect(newBtn, SIGNAL(clicked()), SLOT(slotNewBtn()));
    connect(subBtn, SIGNAL(clicked()), SLOT(slotSubBtn()));

    connect(accountsComboBox, SIGNAL(activated(int)), SLOT(slotActivated(int)));
    
    // Internal hacks, to get renaming to behave along with unique names
    // Uses signals to not get in the way of QListView
    connect(this, SIGNAL(renameStarted(QListViewItem*, int)), SLOT(slotRenameStarted(QListViewItem*, int)));
    connect(this, SIGNAL(startRename(QListViewItem*, int)), SLOT(slotStartRename(QListViewItem*, int)));
    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
}
    
void AccountsPanel::addItems(QListView *lv, Accounts &acc) {
    //kdDebug()<<k_funcinfo<<"No of accs: "<<acc.accountList().count()<<endl;
    AccountListIterator it = acc.accountList();
    for (; it.current(); ++it) {
        QString n = it.current()->name();
        QString d = it.current()->description();
        AccountItem *item = new AccountItem(*this, lv, n, d);
        item->account = it.current();
        item->isDefault = (it.current() == acc.defaultAccount());
        if (it.current()->isElement()) {
            addElement(item);
        }
        addItems(item, it.current());
    }
}

void AccountsPanel::addItems(QListViewItem *item, Account *acc) {
    AccountListIterator it = acc->accountList();
    for (; it.current(); ++it) {
        QString n = it.current()->name();
        QString d = it.current()->description();
        AccountItem *ai = new AccountItem(*this, item, n, d);
        ai->account = it.current();
        ai->isDefault = (it.current() == acc->list()->defaultAccount());
        if (it.current()->isElement()) {
            addElement(ai);
        }
        addItems(ai, it.current());
    }
}

void AccountsPanel::addElement(const QListViewItem *item) {
    if (item->parent()) {
        removeElement(item->parent());
    }
    m_elements.replace(item->text(0), item);
    //kdDebug()<<k_funcinfo<<item->text(0)<<endl;
    refreshDefaultAccount();
}

void AccountsPanel::removeElement(QListViewItem *item) {
    static_cast<AccountItem*>(item)->isDefault = false;
    m_elements.remove(item->text(0));
    refreshDefaultAccount();
}

void AccountsPanel::refreshDefaultAccount() {
    accountsComboBox->clear();
    m_currentIndex = 0;
    accountsComboBox->insertItem(i18n("None"));
    QDictIterator<QListViewItem> it(m_elements);
    for(int i=1; it.current(); ++it, ++i) {
        accountsComboBox->insertItem(it.currentKey());
        if (static_cast<AccountItem*>(it.current())->isDefault) {
            m_currentIndex = i;
            accountsComboBox->setCurrentItem(i);
            //kdDebug()<<k_funcinfo<<"Default="<<it.current()->text(0)<<endl;
        }
    }
    //kdDebug()<<k_funcinfo<<"size="<<accountsComboBox->count()<<endl;
}
void AccountsPanel::slotActivated(int index) {
    //kdDebug()<<k_funcinfo<<index<<endl;
    if (m_currentIndex >= (int)m_elements.count()) {
        kdError()<<k_funcinfo<<"currentIndex ("<<m_currentIndex<<") out of range ("<<m_elements.count()<<")"<<endl;
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
            //kdDebug()<<k_funcinfo<<"currentIndex="<<m_currentIndex<<", "<<m_elements[accountsComboBox->currentText()]->text(0)<<endl;
        }
    }
    slotChanged();
}

void AccountsPanel::slotChanged() {
    emit changed(true);
}

void AccountsPanel::slotSelectionChanged() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_renameItem) {
        removeBtn->setEnabled(false);
        newBtn->setEnabled(false);
        subBtn->setEnabled(false);
        accountList->setSelected(m_renameItem, true);
        return;
    }
    if (accountList->childCount() == 0) {
        removeBtn->setEnabled(false);
        newBtn->setEnabled(true);
        subBtn->setEnabled(false);
        return;
    }
    QListViewItem *i = accountList->selectedItem();
    removeBtn->setEnabled((bool)i);
    newBtn->setEnabled(true);
    subBtn->setEnabled((bool)i);
}

void AccountsPanel::slotItemRenamed(QListViewItem *item, int col) {
    //kdDebug()<<k_funcinfo<<item->text(0)<<endl;
    item->setRenameEnabled(col, false);
    m_renameItem = 0;
    if (col != 0) {
        renameStopped(item);
        slotChanged();
        return;
    }
    if (item->text(0).isEmpty()) {
        item->setText(0, static_cast<AccountItem*>(item)->oldText); // keep the old name
    }
    if (item->text(0).isEmpty()) {
        // Not allowed
        //kdDebug()<<k_funcinfo<<"name empty"<<endl;
        emit startRename(item, 0);
        return;
    }
    if (!isUnique(item)) {
        // name must be unique
        emit startRename(item, 0);
        return;
    }
    addElement(item);
    removeBtn->setEnabled(accountList->selectedItem());
    newBtn->setEnabled(accountList->selectedItem());
    subBtn->setEnabled(accountList->selectedItem());
    renameStopped(item);
    slotChanged();
}

bool AccountsPanel::isUnique(QListViewItem *item) {
    QListViewItemIterator it(accountList);
    for (; it.current(); ++it) {
        if (it.current() != item && it.current()->text(0) == item->text(0)) {
            return false;
        }
    }
    return true;
}

void AccountsPanel::slotRemoveBtn() {
    slotRemoveItem(accountList->selectedItem());
    slotChanged();
}

void AccountsPanel::slotNewBtn() {
    //kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = accountList->selectedItem();
    if (item && item->text(0).isEmpty()) {
        return;
    }
    QListViewItem *n;
    if (item) {
        if (item->parent()) {
            n = new AccountItem(*this, item->parent(), item);
        } else {
            n = new AccountItem(*this, accountList, item);
        }
    } else {
        n = new AccountItem(*this, accountList);
    }
    slotListDoubleClicked(n, QPoint(), 0);
}

void AccountsPanel::slotSubBtn() {
    //kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = accountList->selectedItem();
    if (item && item->text(0).isEmpty()) {
        return;
    }
    QListViewItem *n;
    if (item) {
        n = new AccountItem(*this, item);
    } else {
        n = new AccountItem(*this, accountList);
    }
    slotListDoubleClicked(n, QPoint(), 0);
}

KCommand *AccountsPanel::buildCommand(Part *part) {
    KMacroCommand *cmd = 0;
    // First remove
    QPtrListIterator<QListViewItem> rit = m_removedItems;
    for (;rit.current(); ++rit) {
        AccountItem *item = static_cast<AccountItem*>(rit.current());
        //kdDebug()<<k_funcinfo<<"Removed item"<<endl;
        if (!cmd) cmd = new KMacroCommand(i18n("Modify Accounts"));
        cmd->addCommand(new RemoveAccountCmd(part, part->getProject(), item->account));
    }
    m_removedItems.setAutoDelete(true);
    // Then add/modify
    KCommand *c = save(part, part->getProject());
    if (c) {
        if (!cmd) cmd = new KMacroCommand(i18n("Modify Accounts"));
        cmd->addCommand(c);
    }
    return cmd;
}

KCommand *AccountsPanel::save(Part *part, Project &project) {
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

KCommand *AccountsPanel::save(Part *part, Project &project, QListViewItem *i) {
    KMacroCommand *cmd=0;
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item->account == 0) {
        if (!item->text(0).isEmpty()) {
            //kdDebug()<<k_funcinfo<<"New account: "<<item->text(0)<<endl;
            if (!cmd) cmd = new KMacroCommand("");
            item->account = new Account(item->text(0), item->text(1));
            if (item->parent()) {
                //kdDebug()<<k_funcinfo<<"New account: "<<item->text(0)<<endl;
                cmd->addCommand(new AddAccountCmd(part, project, item->account, item->parent()->text(0)));
            } else {
                cmd->addCommand(new AddAccountCmd(part, project, item->account));
            }
        }
    } else {
        if (!item->text(0).isEmpty() && (item->text(0) != item->account->name())) {
            if (!cmd) cmd = new KMacroCommand("");
            //kdDebug()<<k_funcinfo<<"Renamed: "<<item->account->name()<<" to "<<item->text(0)<<endl;
            cmd->addCommand(new RenameAccountCmd(part, item->account, item->text(0)));
        }
        if (item->text(1) != item->account->description()) {
            if (!cmd) cmd = new KMacroCommand("");
            //kdDebug()<<k_funcinfo<<"New description: "<<item->account->description()<<" to "<<item->text(1)<<endl;
            cmd->addCommand(new ModifyAccountDescriptionCmd(part, item->account, item->text(1)));
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
    AccountItem *ai = static_cast<AccountItem*>(m_elements[accountsComboBox->currentText()]);
    Account *newDefaultAccount = 0;
    if (ai) {
        newDefaultAccount = ai->account;
    }
    if (m_oldDefaultAccount != newDefaultAccount) {
        if (!cmd) cmd = new KMacroCommand("");
        cmd->addCommand(new ModifyDefaultAccountCmd(part, m_accounts, m_oldDefaultAccount, newDefaultAccount));
    }
    return cmd;
}

void AccountsPanel::slotListDoubleClicked(QListViewItem* item, const QPoint&, int col) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (m_renameItem)
        return;
    slotStartRename(item, col);
}

void AccountsPanel::slotRenameStarted(QListViewItem */*item*/, int /*col*/) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (accountList->isRenaming()) {
        removeBtn->setEnabled(false);
        newBtn->setEnabled(false);
        subBtn->setEnabled(false);
    }
}

void AccountsPanel::slotStartRename(QListViewItem *item, int col) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    static_cast<AccountItem*>(item)->oldText = item->text(col);
    item->setRenameEnabled(col, true);
    item->startRename(col);
    m_renameItem = item;
    
    emit renameStarted(item, col);
}

void AccountsPanel::slotRemoveItem(QListViewItem *i) {
    AccountItem *item = static_cast<AccountItem*>(i);
    if (item == 0)
        return;
    //kdDebug()<<k_funcinfo<<item->text(0)<<endl;
    removeElement(item);
    QListViewItem *p = item->parent();
    if (p) {
        p->takeItem(item);
        if (item->account) {
            m_removedItems.append(item);
        } else {
            delete item;
        }
        if (p->childCount() == 0) {
            addElement(p);
        }
    } else {
        accountList->takeItem(item);
        if (item->account) {
            m_removedItems.append(item);
        } else {
            delete item;
        }
    }
}

// We don't get notified when rename is cancelled, this is called from the item
void AccountsPanel::renameStopped(QListViewItem */*item*/) {
    //kdDebug()<<k_funcinfo<<endl;
    m_renameItem = 0;
    emit selectionChanged();
}

void AccountsPanel::slotOk() {

}

} //namespace KPlato

#include "kptaccountspanel.moc"
