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

class QPushButton;
class QTreeWidgetItem;
class QLabel;
class QPushButton;

class QTreeWidget;
class QTreeWidgetItem;
class QPrinter;

class KAction;

namespace KPlato
{

class Account;
class Project;
class Resource;
class ScheduleManager;
class CostBreakdownItemModel;

class KPLATOUI_EXPORT AccountsTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    explicit AccountsTreeView( QWidget *parent = 0 );
    
    CostBreakdownItemModel *model() const;
    
    bool cumulative() const;
    void setCumulative( bool on );
    int periodType() const;
    void setPeriodType( int period );
    int startMode() const;
    void setStartMode( int mode );
    int endMode() const;
    void setEndMode( int mode );
    QDate startDate() const;
    void setStartDate( const QDate &date );
    QDate endDate() const;
    void setEndDate( const QDate &date );
    int showMode() const;
    void setShowMode( int show );

protected slots:
    void slotModelReset();

};

class KPLATOUI_EXPORT AccountsView : public ViewBase
{
    Q_OBJECT
public:
    AccountsView( Project *project, KoDocument *part, QWidget *parent );

    //~AccountsView();
    void setupGui();
    Project *project() const { return m_project; }
    virtual void setZoom( double zoom );
    virtual void setProject( Project *project );

    virtual bool loadContext( const KoXmlElement &context );
    virtual void saveContext( QDomElement &context ) const;

    CostBreakdownItemModel *model() const;

    KoPrintJob *createPrintJob();
    
public slots:
    void setScheduleManager( ScheduleManager *sm );
    
protected slots:
    void slotContextMenuRequested( QModelIndex, const QPoint &pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    virtual void slotOptions();
    
private:
    void init();
    
private:
    Project *m_project;
    ScheduleManager *m_manager;
    AccountsTreeView *m_view;
    
    QDate m_date;
    int m_period;
    bool m_cumulative;
    
};

}  //KPlato namespace

#endif
