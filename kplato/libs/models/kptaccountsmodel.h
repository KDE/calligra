/* This file is KoDocument of the KDE project
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTACCOUNTSMODEL_H
#define KPTACCOUNTSMODEL_H

#include "kplatomodels_export.h"

#include <kptitemmodelbase.h>
#include "kpteffortcostmap.h"

namespace KPlato
{

class Project;
class Account;
class ScheduleManager;
class Node;

class KPLATOMODELS_EXPORT AccountModel : public QObject
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    AccountModel();
    ~AccountModel() {}

    enum Properties {
        Name = 0,
        Description
    };
    const QMetaEnum columnMap() const;
    
    virtual QVariant data( const Account *a, int property, int role = Qt::DisplayRole ) const; 
    virtual QVariant headerData( int property, int role = Qt::DisplayRole ) const; 
    
protected:
    QVariant name( const Account *account, int role ) const;
    QVariant description( const Account *account, int role ) const;
};

class KPLATOMODELS_EXPORT AccountItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit AccountItemModel( QObject *parent = 0 );
    ~AccountItemModel();

    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const Account* account ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    Account *account( const QModelIndex &index ) const;
    QModelIndex insertAccount( Account *account, Account *parent = 0 );
    void removeAccounts( QList<Account*> lst );
    
protected slots:
    void slotAccountChanged( Account* );
    void slotAccountToBeInserted( const Account *parent, int row );
    void slotAccountInserted( const Account *account );
    void slotAccountToBeRemoved( const Account *account );
    void slotAccountRemoved( const Account *account );

protected:
    bool setName( Account *account, const QVariant &value, int role );
    
    bool setDescription( Account *account, const QVariant &value, int role );

private:
    AccountModel m_model;
    Account *m_account; // test for sane operation
};

//---------------
class KPLATOMODELS_EXPORT CostBreakdownItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    enum PeriodType { Period_Day = 0, Period_Week = 1, Period_Month = 2 };
    enum StartMode { StartMode_Project = 0, StartMode_Date = 1 };
    enum EndMode { EndMode_Project = 0, EndMode_Date = 1, EndMode_CurrentDate = 2 };
    enum ShowMode { ShowMode_Actual = 0, ShowMode_Planned = 1, ShowMode_Both = 2, ShowMode_Deviation = 3 };
    
    explicit CostBreakdownItemModel( QObject *parent = 0 );
    ~CostBreakdownItemModel();

    virtual void setProject( Project *project );
    virtual void setScheduleManager( ScheduleManager *sm );
    long id() const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const Account* account ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    Account *account( const QModelIndex &index ) const;

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

    QString formatMoney( double plannedCost, double actualCost ) const;
    QString format() const { return m_format; }
    void setFormat( const QString &f ) { m_format = f; }
    
protected:
    void fetchData();
    EffortCostMap fetchPlannedCost( Account *account );
    EffortCostMap fetchActualCost( Account *account );
    
protected slots:
    void slotAccountChanged( Account* );
    void slotAccountToBeInserted( const Account *parent, int row );
    void slotAccountInserted( const Account *account );
    void slotAccountToBeRemoved( const Account *account );
    void slotAccountRemoved( const Account *account );
    
    void slotNodeChanged( Node *node );

private:
    ScheduleManager *m_manager;
    
    bool m_cumulative;
    int m_periodtype;
    int m_startmode;
    int m_endmode;
    QDate m_start;
    QDate m_end;
    int m_showmode;
    QMap<Account*, EffortCostMap> m_plannedCostMap;
    QDate m_plannedStart, m_plannedEnd;
    QMap<Account*, EffortCostMap> m_actualCostMap;
    QDate m_actualStart, m_actualEnd;
    QString m_format;
    
};


}  //KPlato namespace

#endif
