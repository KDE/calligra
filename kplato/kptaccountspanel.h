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

class KPTAccount;
class KPTAccounts;
class KPTPart;
class KPTProject;

class KPTAccountsPanel : public KPTAccountsPanelBase {
    Q_OBJECT
public:
    KPTAccountsPanel(KPTAccounts &acc, QWidget *parent=0, const char *name=0);
    
    KCommand *buildCommand(KPTPart *part);
    
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
    void addItems(QListView *lv, KPTAccounts &acc);
    void addItems(QListViewItem *item, KPTAccount *acc);
    KCommand *save(KPTPart *part, KPTProject &project);
    KCommand *save(KPTPart *part, KPTProject &project, QListViewItem *item);
    
private:
    KPTAccounts &m_accounts;
    
    QPtrList<QListViewItem> m_removedItems;
};

} //namespace KPlato

#endif
