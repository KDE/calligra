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

#ifndef KPTACCOUNTSPANEL_H
#define KPTACCOUNTSPANEL_H

#include "kplatoui_export.h"

#include "ui_kptaccountspanelbase.h"

#include <QList>
#include <QHash>

class QTreeWidget;
class QTreeWidgetItem;
class QWidget;


namespace KPlato
{

class Account;
class Accounts;
class Project;
class MacroCommand;

class AccountsPanelBase : public QWidget, public Ui::AccountsPanelBase
{
public:
  explicit AccountsPanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};


class KPLATOUI_EXPORT AccountsPanel : public AccountsPanelBase {
    Q_OBJECT
public:
    explicit AccountsPanel(Project &project, Accounts &acc, QWidget *parent=0);
    
    MacroCommand *buildCommand();
    
    bool isUnique(QTreeWidgetItem *item);
    void renameStopped(QTreeWidgetItem *item);
    
signals:
    void changed(bool);
    
public slots:
    void slotOk();
    
protected slots:
    void slotChanged();
    void slotSelectionChanged();
    void slotRemoveBtn();
    void slotNewBtn();
    void slotSubBtn();
    void slotActivated(int);
    void slotItemChanged(QTreeWidgetItem* item, int col);
    void slotRemoveItem(QTreeWidgetItem *i);
    
protected:
    void addItems(QTreeWidget *lv, Accounts &acc);
    void addItems(QTreeWidgetItem *item, Account *acc);
    void addElement(QTreeWidgetItem *item);
    void removeElement(const QString& key);
    void removeElement(QTreeWidgetItem *item);
    void refreshDefaultAccount();
    MacroCommand *save(Project &project);
    MacroCommand *save(Project &project, QTreeWidgetItem *item);
    
private:
    Project &m_project;
    Accounts &m_accounts;
    
    QList<QTreeWidgetItem*> m_removedItems;
    Account *m_oldDefaultAccount;
    QHash<QString, QTreeWidgetItem*> m_elements;
    int m_currentIndex;
    QString m_renameText;
    QTreeWidgetItem *m_renameItem;
};

} //namespace KPlato

#endif
