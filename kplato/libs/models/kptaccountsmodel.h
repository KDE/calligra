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
    enum PeriodType { Period_Day, Period_Week, Period_Month };
    
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

    void fetchData();
    void setPeriodType( int period );
    void setPeriod( const QDate &start, const QDate &end );
    
protected slots:
    void slotAccountChanged( Account* );
    void slotAccountToBeInserted( const Account *parent, int row );
    void slotAccountInserted( const Account *account );
    void slotAccountToBeRemoved( const Account *account );
    void slotAccountRemoved( const Account *account );

private:
    Project *m_project;
    ScheduleManager *m_manager;
    
    int m_period;
    QDate m_start;
    QDate m_end;
    QMap<Account*, EffortCostMap> m_plannedCostMap;
    QMap<Account*, EffortCostMap> m_actualCostMap;

};


}  //KPlato namespace

#endif
