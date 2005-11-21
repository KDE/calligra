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

class QListView;
class QListViewItem;
class QWidget;

class KCommand;
class KMacroCommand;

namespace KPlato
{

class Account;
class Accounts;
class Part;
class Project;

class AccountsPanel : public AccountsPanelBase {
    Q_OBJECT
public:
    AccountsPanel(Accounts &acc, QWidget *parent=0, const char *name=0);
    
    KCommand *buildCommand(Part *part);
    
signals:
    void changed(bool);
    
public slots:
    void slotOk();
    
protected slots:
    void slotChanged();
    void slotSelectionChanged();
    void slotItemRenamed(QListViewItem *item, int col);
    void slotRemoveBtn();
    void slotNewBtn();
    void slotSubBtn();
    
protected:
    void addItems(QListView *lv, Accounts &acc);
    void addItems(QListViewItem *item, Account *acc);
    KCommand *save(Part *part, Project &project);
    KCommand *save(Part *part, Project &project, QListViewItem *item);
    
private:
    Accounts &m_accounts;
    
    QPtrList<QListViewItem> m_removedItems;
};

} //namespace KPlato

#endif
