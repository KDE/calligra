/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <kplato@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPTACCOUNTSVIEW_H
#define KPTACCOUNTSVIEW_H

#include <qsplitter.h>
#include <qdatetime.h>

#include <klistview.h>

#include "kptaccount.h"
#include "kptcontext.h"
#include "kpteffortcostmap.h"

class QComboBox;
class QDateEdit;
class QPushButton;
class QSplitter;
class QListViewItem;
class QLabel;
class QPushButton;

class KListView;
class KListViewItem;
class KPrinter;

namespace KPlato
{

class Account;
class View;
class Project;
class Resource;
class Node;

class ResourceGroup;
class Resource;
class ResourceItemPrivate;

class AccountItem;

 class AccountsView : public QWidget
{
    Q_OBJECT
public:

    AccountsView(Project &project, View *view, QWidget *parent);

    //~AccountsView();

    void zoom(double zoom);

    View *mainView() { return m_mainview; }
    void draw();
    void print(KPrinter &printer);
    
    virtual bool setContext(Context::Accountsview &context);
    virtual void getContext(Context::Accountsview &context) const;

    class AccountItem;
    class AccountPeriodItem : public KListViewItem {
    public:
        AccountPeriodItem(AccountItem *o, QListView *parent, QListViewItem *after, bool highlight=false);
        AccountPeriodItem(AccountItem *o, QListViewItem *parent, QListViewItem *after, bool _highlight=false);
        ~AccountPeriodItem();
    
        void setColumn(int col, double cost);
        void clearColumn(int col);
                
        AccountItem *owner;
        double value;
        bool highlight;
        
        QMap<int, double> costMap;
        
        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
    };

    class AccountItem : public KListViewItem {
    public:
        AccountItem(Account *a, QListView *parent, bool highlight=false);
        AccountItem(Account *a, QListViewItem *parent, bool highlight=false);
        AccountItem(QString text, Account *a, QListViewItem *parent, bool _highlight=false);
        ~AccountItem();
        
        void createPeriods(QListView *lv, QListViewItem *after=0);
        void periodDeleted();
        void setTotal(double tot);
        double calcTotal();
        void add(int col, const QDate &date, const EffortCost &ec);
        void addToTotal(double v);
        void setPeriod(int col, double cost);
        void clearColumn(int col);
        void calcPeriods();
        double calcPeriod(int col);
        
        Account *account;
        AccountPeriodItem *period;
        double value;
        bool highlight;
        EffortCostMap costMap;
        
        
        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);    
    
    };

signals:
    void update();
    
public slots:
    void slotConfigure();

protected slots:
    void slotUpdate();
    void slotExpanded(QListViewItem* item);
    void slotCollapsed(QListViewItem* item);
    
private:
    void init();
    void initAccList(const AccountList &list);
    void initAccSubItems(Account *acc, AccountItem *parent);
    void initAccList(const AccountList &list, AccountItem *parent);
    void createPeriods();
    void clearPeriods();
    QString periodText(int offset);
    
private:
    View *m_mainview;
    Project &m_project;
    Accounts &m_accounts;
    
    int m_defaultFontSize;

    KListView *m_accList;
    KListView *m_periodList;
    
    QDate m_date;
    int m_period;
    bool m_cumulative;
    QSplitter *m_splitter;

    QStringList m_periodTexts;
    QPushButton *m_changeBtn;
    QLabel *m_label;
    
};

}  //KPlato namespace

#endif
