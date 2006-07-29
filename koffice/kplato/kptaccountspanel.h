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

#ifndef KPTACCOUNTSPANEL_H
#define KPTACCOUNTSPANEL_H

#include "kptaccountspanelbase.h"

#include <qptrlist.h>
#include <qdict.h>

class QListView;
class QListViewItem;
class QWidget;

class KCommand;
class KMacroCommand;

namespace KPlato
{

class AccountItem;
class Account;
class Accounts;
class Part;
class Project;

class AccountsPanel : public AccountsPanelBase {
    Q_OBJECT
public:
    AccountsPanel(Accounts &acc, QWidget *parent=0, const char *name=0);
    
    KCommand *buildCommand(Part *part);
    
    bool isUnique(QListViewItem *item);
    void renameStopped(QListViewItem *item);
    
signals:
    void changed(bool);
    
    // Internal
    void renameStarted(QListViewItem *, int);
    void startRename(QListViewItem *item, int col);
    void selectionChanged();
    
public slots:
    void slotOk();
    
protected slots:
    void slotChanged();
    void slotSelectionChanged();
    void slotItemRenamed(QListViewItem *item, int col);
    void slotRemoveBtn();
    void slotNewBtn();
    void slotSubBtn();
    void slotActivated(int);
    void slotListDoubleClicked(QListViewItem* item, const QPoint&, int col);
    void slotRenameStarted(QListViewItem *item, int col);
    void slotStartRename(QListViewItem *item, int col);
    void slotRemoveItem(QListViewItem *i);
protected:
    void addItems(QListView *lv, Accounts &acc);
    void addItems(QListViewItem *item, Account *acc);
    void addElement(const QListViewItem *item);
    void removeElement(QListViewItem *item);
    void refreshDefaultAccount();
    KCommand *save(Part *part, Project &project);
    KCommand *save(Part *part, Project &project, QListViewItem *item);
    
private:
    Accounts &m_accounts;
    
    QPtrList<QListViewItem> m_removedItems;
    Account *m_oldDefaultAccount;
    QDict<QListViewItem> m_elements;
    int m_currentIndex;
    QString m_renameText;
    QListViewItem *m_renameItem;
};

} //namespace KPlato

#endif
