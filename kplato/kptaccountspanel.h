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

#include "ui_kptaccountspanelbase.h"

#include <QList>
#include <QHash>

class Q3ListView;
class Q3ListViewItem;
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

class AccountsPanelBase : public QWidget, public Ui::AccountsPanelBase
{
public:
  AccountsPanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};


class AccountsPanel : public AccountsPanelBase {
    Q_OBJECT
public:
    AccountsPanel(Accounts &acc, QWidget *parent=0);
    
    KCommand *buildCommand(Part *part);
    
    bool isUnique(Q3ListViewItem *item);
    void renameStopped(Q3ListViewItem *item);
    
signals:
    void changed(bool);
    
    // Internal
    void renameStarted(Q3ListViewItem *, int);
    void startRename(Q3ListViewItem *item, int col);
    void selectionChanged();
    
public slots:
    void slotOk();
    
protected slots:
    void slotChanged();
    void slotSelectionChanged();
    void slotItemRenamed(Q3ListViewItem *item, int col);
    void slotRemoveBtn();
    void slotNewBtn();
    void slotSubBtn();
    void slotActivated(int);
    void slotListDoubleClicked(Q3ListViewItem* item, const QPoint&, int col);
    void slotRenameStarted(Q3ListViewItem *item, int col);
    void slotStartRename(Q3ListViewItem *item, int col);
    void slotRemoveItem(Q3ListViewItem *i);
protected:
    void addItems(Q3ListView *lv, Accounts &acc);
    void addItems(Q3ListViewItem *item, Account *acc);
    void addElement(Q3ListViewItem *item);
    void removeElement(QString key);
    void removeElement(Q3ListViewItem *item);
    void refreshDefaultAccount();
    KCommand *save(Part *part, Project &project);
    KCommand *save(Part *part, Project &project, Q3ListViewItem *item);
    
private:
    Accounts &m_accounts;
    
    QList<Q3ListViewItem*> m_removedItems;
    Account *m_oldDefaultAccount;
    QHash<QString, Q3ListViewItem*> m_elements;
    int m_currentIndex;
    QString m_renameText;
    Q3ListViewItem *m_renameItem;
};

} //namespace KPlato

#endif
