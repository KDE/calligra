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

#include <qdatetime.h>
//Added by qt3to4:
#include <QLabel>

#include <k3listview.h>

#include "kptaccount.h"
#include "kptcontext.h"
#include "kpteffortcostmap.h"
#include "kptdoublelistviewbase.h"

class QComboBox;
class Q3DateEdit;
class QPushButton;
class QSplitter;
class Q3ListViewItem;
class QLabel;
class QPushButton;

class K3ListView;
class K3ListViewItem;
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

signals:
    void update();

public slots:
    void slotConfigure();

protected slots:
    void slotUpdate();

protected:
    void getContextClosedItems(Context::Accountsview &context, Q3ListViewItem *item) const;
    void setContextClosedItems(Context::Accountsview &context);

private:
    class AccountItem : public DoubleListViewBase::MasterListItem {
    public:
        AccountItem(Account *a, Q3ListView *parent, bool highlight=false);
        AccountItem(Account *a, Q3ListViewItem *parent, bool highlight=false);
        AccountItem(QString text, Account *a, Q3ListViewItem *parent, bool _highlight=false);

        void add(int col, const QDate &date, const EffortCost &ec);

        Account *account;
        EffortCostMap costMap;
    };

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

    QDate m_date;
    int m_period;
    bool m_cumulative;

    DoubleListViewBase *m_dlv;

    QStringList m_periodTexts;
    QPushButton *m_changeBtn;
    QLabel *m_label;

};

}  //KPlato namespace

#endif
