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

#include "kplatoui_export.h"

#include "kptviewbase.h"

#include <KoDocument.h>

#include <QDate>
#include <QLabel>

#include "kptaccount.h"
#include "kpteffortcostmap.h"
#include "kptdoublelistviewbase.h"

class QPushButton;
class QTreeWidgetItem;
class QLabel;
class QPushButton;

class QTreeWidget;
class QTreeWidgetItem;
class QPrinter;

namespace KPlato
{

class Account;
class Project;
class Resource;
class ScheduleManager;

class KPLATOUI_EXPORT AccountsView : public ViewBase
{
    Q_OBJECT
public:

    AccountsView( Project *project, KoDocument *part, QWidget *parent );

    //~AccountsView();

    virtual void setZoom( double zoom );
    virtual void setProject( Project *project );
    using ViewBase::draw;
    virtual void draw();

    virtual bool loadContext( const KoXmlElement &context );
    virtual void saveContext( QDomElement &context ) const;

signals:
    void configChanged();

public slots:
    void slotConfigure();
    void setScheduleManager( ScheduleManager *sm );
    
protected slots:
    void slotUpdate();
    
protected:
//    void getContextClosedItems( Context::Accountsview &context, QTreeWidgetItem *item ) const;
//    void setContextClosedItems( const Context::Accountsview &context );

private:
    class KPLATOUI_EXPORT AccountItem : public DoubleListViewBase::MasterListItem
    {
    public:
        AccountItem( Account *a, QTreeWidget *parent, bool highlight = false );
        AccountItem( Account *a, QTreeWidgetItem *parent, bool highlight = false );
        AccountItem( const QString& text, Account *a, QTreeWidgetItem *parent, bool _highlight = false );

        void add
            ( int col, const QDate &date, const EffortCost &ec );

        Account *account;
        EffortCostMap costMap;
    };
    
    void init();
    void initAccList( const AccountList &list );
    void initAccSubItems( Account *acc, AccountItem *parent );
    void initAccList( const AccountList &list, AccountItem *parent );
    void createPeriods();
    void clearPeriods();
    QString periodText( int offset );

private:
    Project *m_project;
    ScheduleManager *m_manager;

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
