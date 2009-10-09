/* This file is part of the KDE project
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

#include "kptaccountsmodel.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptaccount.h"
#include "kptdatetime.h"
#include "kptschedule.h"

#include <QList>
#include <QObject>


#include <kglobal.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

//--------------------------------------
AccountModel::AccountModel()
    : QObject()
{
}

const QMetaEnum AccountModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

QVariant AccountModel::data( const Account *a, int property, int role ) const
{
    QVariant result;
    if ( a == 0 ) {
        return QVariant();
    }
    switch ( property ) {
        case AccountModel::Name: result = name( a, role ); break;
        case AccountModel::Description: result = description( a, role ); break;
        default:
            kDebug()<<"data: invalid display value column"<<property;
            return QVariant();
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    // define default action

    return QVariant();
}

QVariant AccountModel::name( const Account *a, int role ) const
{
    //kDebug()<<a->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return a->name();
        case Qt::ToolTipRole:
            if ( a->isDefaultAccount() ) {
                return i18nc( "1=account name", "%1 (Default account)", a->name() );
            }
            return a->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
         case Qt::CheckStateRole:
             return a->isDefaultAccount() ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

QVariant AccountModel::description( const Account *a, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return a->description();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant AccountModel::headerData( int property, int role ) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( property ) {
            case AccountModel::Name: return i18n( "Name" );
            case AccountModel::Description: return i18n( "Description" );
            default: return QVariant();
        }
    }
    if ( role == Qt::TextAlignmentRole ) {
        switch (property) {
            case AccountModel::Description: return Qt::AlignLeft;
            default: return QVariant();
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( property ) {
            case AccountModel::Name: return ToolTip::accountName();
            case AccountModel::Description: return ToolTip::accountDescription();
            default: return QVariant();
        }
    }
    return QVariant();
}

//----------------------------------------
AccountItemModel::AccountItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_account( 0 )
{
}

AccountItemModel::~AccountItemModel()
{
}

void AccountItemModel::slotAccountToBeInserted( const Account *parent, int row )
{
    //kDebug()<<parent->name();
    Q_ASSERT( m_account == 0 );
    m_account = const_cast<Account*>(parent);
    beginInsertRows( index( parent ), row, row );
}

void AccountItemModel::slotAccountInserted( const Account *account )
{
    //kDebug()<<account->name();
    Q_ASSERT( account->parent() == m_account );
    endInsertRows();
    m_account = 0;
}

void AccountItemModel::slotAccountToBeRemoved( const Account *account )
{
    //kDebug()<<account->name();
    Q_ASSERT( m_account == 0 );
    m_account = const_cast<Account*>(account);
    int row = index( account ).row();
    beginRemoveRows( index( account->parent() ), row, row );
}

void AccountItemModel::slotAccountRemoved( const Account *account )
{
    //kDebug()<<account->name();
    Q_ASSERT( account == m_account );
    endRemoveRows();
    m_account = 0;
}

void AccountItemModel::setProject( Project *project )
{
    if ( m_project ) {
        Accounts *acc = &( m_project->accounts() );
        disconnect( acc , SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        disconnect( acc, SIGNAL( accountAdded( const Account* ) ), this, SLOT( slotAccountInserted( const Account* ) ) );
        disconnect( acc, SIGNAL( accountToBeAdded( const Account*, int ) ), this, SLOT( slotAccountToBeInserted( const Account*, int ) ) );
        
        disconnect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        disconnect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    }
    m_project = project;
    if ( project ) {
        Accounts *acc = &( project->accounts() );
        kDebug()<<acc;
        connect( acc, SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        connect( acc, SIGNAL( accountAdded( const Account* ) ), this, SLOT( slotAccountInserted( const Account* ) ) );
        connect( acc, SIGNAL( accountToBeAdded( const Account*, int ) ), this, SLOT( slotAccountToBeInserted( const Account*, int ) ) );
        
        connect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        connect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    }
}

Qt::ItemFlags AccountItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( !index.isValid() ) {
        return flags;
    }
    if ( account ( index ) ) {
        switch ( index.column() ) {
            case AccountModel::Name: flags |= ( Qt::ItemIsEditable | Qt::ItemIsUserCheckable ); break;
            default: flags |= Qt::ItemIsEditable; break;
        }
    }
    return flags;
}


QModelIndex AccountItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    Account *a = account( index );
    if ( a == 0 ) {
        return QModelIndex();
    }
    Account *par = a->parent();
    if ( par ) {
        a = par->parent();
        int row = -1;
        if ( a ) {
            row = a->accountList().indexOf( par );
        } else {
            row = m_project->accounts().accountList().indexOf( par );
        }
        //kDebug()<<par->name()<<":"<<row;
        return createIndex( row, 0, par );
    }
    return QModelIndex();
}

QModelIndex AccountItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        if ( row < m_project->accounts().accountList().count() ) {
            return createIndex( row, column, m_project->accounts().accountList().at( row ) );
        }
    } else if ( row < par->accountList().count() ) {
        return createIndex( row, column, par->accountList().at( row ) );
    }
    return QModelIndex();
}

QModelIndex AccountItemModel::index( const Account *account ) const
{
    Account *a = const_cast<Account*>(account);
    if ( m_project == 0 || account == 0 ) {
        return QModelIndex();
    }
    int row = -1;
    Account *par = a->parent();
    if ( par == 0 ) {
         row = m_project->accounts().accountList().indexOf( a );
    } else {
        row = par->accountList().indexOf( a );
    }
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, a );

}

int AccountItemModel::columnCount( const QModelIndex & ) const
{
    return 2;
}

int AccountItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        return m_project->accounts().accountList().count();
    }
    return par->accountList().count();
}

bool AccountItemModel::setName( Account *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() != a->name() ) {
                emit executeCommand( new RenameAccountCmd( a, value.toString(), "Modify account name" ) );
            }
            return true;
        case Qt::CheckStateRole: {
            switch ( value.toInt() ) {
                case Qt::Unchecked:
                    if ( a->isDefaultAccount() ) {
                        emit executeCommand( new ModifyDefaultAccountCmd( m_project->accounts(), a, 0, i18n( "De-select as default account" ) ) );
                        return true;
                    }
                    break;
                case Qt::Checked:
                    if ( ! a->isDefaultAccount() ) {
                        emit executeCommand( new ModifyDefaultAccountCmd( m_project->accounts(), m_project->accounts().defaultAccount(), a, i18n( "Select as default account" ) ) );
                        return true;
                    }
                    break;
                default: break;
            }
        }
        default: break;
    }
    return false;
}

bool AccountItemModel::setDescription( Account *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() != a->description() ) {
                emit executeCommand( new ModifyAccountDescriptionCmd( a, value.toString(), "Modify account description" ) );
            }
            return true;
    }
    return false;
}

QVariant AccountItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Account *a = account( index );
    if ( a == 0 ) {
        return QVariant();
    }
    result = m_model.data( a, index.column(), role );
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
    }
    return result;
}

bool AccountItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags( index ) &( Qt::ItemIsEditable | Qt::CheckStateRole ) ) == 0 ) {
        Q_ASSERT( true );
        return false;
    }
    Account *a = account( index );
    kDebug()<<a->name()<<value<<role;
    switch (index.column()) {
        case AccountModel::Name: return setName( a, value, role );
        case AccountModel::Description: return setDescription( a, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant AccountItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        return m_model.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

Account *AccountItemModel::account( const QModelIndex &index ) const
{
    return static_cast<Account*>( index.internalPointer() );
}

void AccountItemModel::slotAccountChanged( Account *account )
{
    Account *par = account->parent();
    if ( par ) {
        int row = par->accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    } else {
        int row = m_project->accounts().accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    }
}

QModelIndex AccountItemModel::insertAccount( Account *account, Account *parent )
{
    kDebug();
    if ( account->name().isEmpty() || m_project->accounts().findAccount( account->name() ) ) {
        QString s = parent == 0 ? account->name() : parent->name();
        account->setName( m_project->accounts().uniqueId( s ) );
        //m_project->accounts().insertId( account );
    }
    emit executeCommand( new AddAccountCmd( *m_project, account, parent, i18n( "Add account" ) ) );
    int row = -1;
    if ( parent ) {
        row = parent->accountList().indexOf( account );
    } else {
        row = m_project->accounts().accountList().indexOf( account );
    }
    if ( row != -1 ) {
        //kDebug()<<"Inserted:"<<account->name();
        return createIndex( row, 0, account );
    }
    kDebug()<<"Can't find"<<account->name();
    return QModelIndex();
}

void AccountItemModel::removeAccounts( QList<Account*> lst )
{
    MacroCommand *cmd = 0;
    QString c = i18np( "Delete Account", "Delete %1 Accounts", lst.count() );
    while ( ! lst.isEmpty() ) {
        bool del = true;
        Account *acc = lst.takeFirst();
        foreach ( Account *a, lst ) {
            if ( acc->isChildOf( a ) ) {
                del = false; // acc will be deleted when a is deleted
                break;
            }
        }
        if ( del ) {
            if ( cmd == 0 ) cmd = new MacroCommand( c );
            cmd->addCommand( new RemoveAccountCmd( *m_project, acc ) );
        }
    }
    if ( cmd )
        emit executeCommand( cmd );
}

//----------------------------------------
CostBreakdownItemModel::CostBreakdownItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 ),
    m_cumulative( false ),
    m_periodtype( Period_Day ),
    m_startmode( StartMode_Project ),
    m_endmode( EndMode_Project ),
    m_showmode( ShowMode_Both )
{
    m_format = QString( "%1 [%2]" );
}

CostBreakdownItemModel::~CostBreakdownItemModel()
{
}

void CostBreakdownItemModel::slotAccountToBeInserted( const Account *parent, int row )
{
    //kDebug()<<parent->name();
    beginInsertRows( index( parent ), row, row );
}

void CostBreakdownItemModel::slotAccountInserted( const Account *account )
{
    //kDebug()<<account->name();
    endInsertRows();
}

void CostBreakdownItemModel::slotAccountToBeRemoved( const Account *account )
{
    //kDebug()<<account->name();
    int row = index( account ).row();
    beginRemoveRows( index( account->parent() ), row, row );
}

void CostBreakdownItemModel::slotAccountRemoved( const Account *account )
{
    //kDebug()<<account->name();
    endRemoveRows();
}

void CostBreakdownItemModel::slotNodeChanged( Node *node )
{
    fetchData();
    foreach ( Account *a, m_plannedCostMap.keys() ) {
        QModelIndex idx1 = index( a );
        QModelIndex idx2 = index( idx1.row(), columnCount() - 1, parent( idx1 ) );
        //kDebug()<<a->name()<<idx1<<idx2;
        emit dataChanged( idx1, idx2  );
    }
}

void CostBreakdownItemModel::setProject( Project *project )
{
    if ( m_project ) {
        Accounts *acc = &( m_project->accounts() );
        disconnect( acc , SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        disconnect( acc, SIGNAL( accountAdded( const Account* ) ), this, SLOT( slotAccountInserted( const Account* ) ) );
        disconnect( acc, SIGNAL( accountToBeAdded( const Account*, int ) ), this, SLOT( slotAccountToBeInserted( const Account*, int ) ) );
        
        disconnect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        disconnect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    
        disconnect( m_project , SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project , SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project , SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    m_project = project;
    if ( project ) {
        Accounts *acc = &( project->accounts() );
        kDebug()<<acc;
        connect( acc, SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        connect( acc, SIGNAL( accountAdded( const Account* ) ), this, SLOT( slotAccountInserted( const Account* ) ) );
        connect( acc, SIGNAL( accountToBeAdded( const Account*, int ) ), this, SLOT( slotAccountToBeInserted( const Account*, int ) ) );
        
        connect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        connect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    
        connect( project , SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( project , SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( project , SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
}

void CostBreakdownItemModel::setScheduleManager( ScheduleManager *sm )
{
    kDebug()<<m_project<<m_manager<<sm;
    if ( m_manager != sm ) {
        m_manager = sm;
        fetchData();
        reset();
    }
}

long CostBreakdownItemModel::id() const
{
    return m_manager == 0 ? -1 : m_manager->id();
}

EffortCostMap CostBreakdownItemModel::fetchPlannedCost( Account *account )
{
    EffortCostMap ec;
    if ( account->isElement() ) {
        ec = account->plannedCost( id() );
    } else {
        foreach ( Account *a, account->accountList() ) {
            ec += fetchPlannedCost( a );
        }
    }
    m_plannedCostMap.insert( account, ec );
    QDate s = ec.startDate();
    if ( ! m_plannedStart.isValid() || s < m_plannedStart ) {
        m_plannedStart = s;
    }
    QDate e = ec.endDate();
    if ( ! m_plannedEnd.isValid() || e > m_plannedEnd ) {
        m_plannedEnd = e;
    }
    return ec;
}

EffortCostMap CostBreakdownItemModel::fetchActualCost( Account *account )
{
    qDebug()<<"fetchActualCost:"<<account->name();
    EffortCostMap ec;
    if ( account->isElement() ) {
        ec = account->actualCost( id() );
    } else {
        foreach ( Account *a, account->accountList() ) {
            ec += fetchActualCost( a );
        }
    }
    m_actualCostMap.insert( account, ec );
    QDate s = ec.startDate();
    if ( ! m_actualStart.isValid() || s < m_actualStart ) {
        m_actualStart = s;
    }
    QDate e = ec.endDate();
    if ( ! m_actualEnd.isValid() || e > m_actualEnd ) {
        m_actualEnd = e;
    }
    qDebug()<<"fetchActualCost:"<<account->name()<<ec.totalEffort().toDouble(Duration
::Unit_h)<<ec.totalCost();
    return ec;
}

void CostBreakdownItemModel::fetchData()
{
    //kDebug()<<m_start<<m_end;
    m_plannedCostMap.clear();
    m_plannedStart = m_plannedEnd = QDate();
    m_actualStart = m_actualEnd = QDate();
    if ( m_project == 0 || m_manager == 0 ) {
        return;
    }
    foreach ( Account *a, m_project->accounts().accountList() ) {
        fetchPlannedCost( a );
        fetchActualCost( a );
    }
}

QModelIndex CostBreakdownItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    Account *a = account( index );
    if ( a == 0 ) {
        return QModelIndex();
    }
    Account *par = a->parent();
    if ( par ) {
        a = par->parent();
        int row = -1;
        if ( a ) {
            row = a->accountList().indexOf( par );
        } else {
            row = m_project->accounts().accountList().indexOf( par );
        }
        //kDebug()<<par->name()<<":"<<row;
        return createIndex( row, 0, par );
    }
    return QModelIndex();
}

QModelIndex CostBreakdownItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        if ( row < m_project->accounts().accountList().count() ) {
            return createIndex( row, column, m_project->accounts().accountList().at( row ) );
        }
    } else if ( row < par->accountList().count() ) {
        return createIndex( row, column, par->accountList().at( row ) );
    }
    return QModelIndex();
}

QModelIndex CostBreakdownItemModel::index( const Account *account ) const
{
    Account *a = const_cast<Account*>(account);
    if ( m_project == 0 || account == 0 ) {
        return QModelIndex();
    }
    int row = -1;
    Account *par = a->parent();
    if ( par == 0 ) {
        row = m_project->accounts().accountList().indexOf( a );
    } else {
        row = par->accountList().indexOf( a );
    }
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, a );

}

int CostBreakdownItemModel::columnCount( const QModelIndex & ) const
{
    int c = 3;
    if ( startDate().isValid() && endDate().isValid() ) {
        switch ( m_periodtype ) {
            case Period_Day: {
                c += startDate().daysTo( endDate()) + 1;
                break;
            }
            case Period_Week: {
                int days = KGlobal::locale()->weekStartDay() - startDate().dayOfWeek();
                if ( days > 0 ) {
                    days -= 7;
                }
                QDate start = startDate().addDays( days );
                c += (start.daysTo( endDate() ) / 7) + 1;
                break;
            }
            case Period_Month: {
                int days = startDate().daysInMonth() - startDate().day() + 1;
                for ( QDate d = startDate(); d < endDate(); d = d.addDays( days ) ) {
                    ++c;
                    days = qMin( d.daysTo( endDate() ), d.daysInMonth() );
                }
                break;
            }
        }
    }
    return c;
}

int CostBreakdownItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        return m_project->accounts().accountList().count();
    }
    return par->accountList().count();
}

QString CostBreakdownItemModel::formatMoney( double cost1, double cost2 ) const
{
    if ( m_showmode == ShowMode_Planned ) {
        return m_project->locale()->formatMoney( cost1, "", 0 );
    }
    if ( m_showmode == ShowMode_Actual ) {
        return m_project->locale()->formatMoney( cost2, "", 0 );
    }
    if ( m_showmode == ShowMode_Both ) {
        return QString(m_format).arg( m_project->locale()->formatMoney( cost2, "", 0 ) ).arg( m_project->locale()->formatMoney( cost1, "", 0 ) );
    }
    if ( m_showmode == ShowMode_Deviation ) {
        return m_project->locale()->formatMoney( cost1 - cost2, "", 0 );
    }
    return "";
}

QVariant CostBreakdownItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Account *a = account( index );
    if ( a == 0 ) {
        return QVariant();
    }
    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
            case 0: return a->name();
            case 1: return a->description();
            case 2: {
                return formatMoney( m_plannedCostMap.value( a ).totalCost(), m_actualCostMap.value( a ).totalCost() );
            }
            default: {
                int col = index.column() - 3;
                EffortCostMap pc = m_plannedCostMap.value( a );
                EffortCostMap ac = m_actualCostMap.value( a );
                switch ( m_periodtype ) {
                    case Period_Day: {
                        double planned = 0.0;
                        if ( m_cumulative ) {
                            planned = pc.costTo( startDate().addDays( col ) );
                        } else {
                            planned = pc.costOnDate( startDate().addDays( col ) );
                        }
                        double actual = 0.0;
                        if ( m_cumulative ) {
                            actual = ac.costTo( startDate().addDays( col ) );
                        } else {
                            actual = ac.costOnDate( startDate().addDays( col ) );
                        }
                        return formatMoney( planned, actual );
                    }
                    case Period_Week: {
                        int days = KGlobal::locale()->weekStartDay() - startDate().dayOfWeek();
                        if ( days > 0 ) {
                            days -= 7; ;
                        }
                        QDate start = startDate().addDays( days );
                        int week = col;
                        double planned = 0.0;
                        if ( m_cumulative ) {
                            planned = pc.costTo( start.addDays( ++week * 7 ) );
                        } else {
                            planned = week == 0 ? pc.cost( startDate(), startDate().daysTo( start.addDays( 7 ) ) ) : pc.cost( start.addDays( week * 7 ) );
                        }
                        double actual = 0.0;
                        if ( m_cumulative ) {
                            actual = ac.costTo( start.addDays( ++week * 7 ) );
                        } else {
                            actual = week == 0 ? ac.cost( startDate(), startDate().daysTo( start.addDays( 7 ) ) ) : ac.cost( start.addDays( week * 7 ) );
                        }
                        return formatMoney( planned, actual );
                    }
                    case Period_Month: {
                        int days = startDate().daysInMonth() - startDate().day() + 1;
                        QDate start = startDate();
                        for ( int i = 0; i < col; ++i ) {
                            start = start.addDays( days );
                            days = start.daysInMonth();
                        }
                        int planned = 0.0;
                        if ( m_cumulative ) {
                            planned = pc.costTo( start.addDays( start.daysInMonth() - start.day() + 1 ) );
                        } else {
                            planned = pc.cost( start, start.daysInMonth() - start.day() + 1);
                        }
                        int actual = 0.0;
                        if ( m_cumulative ) {
                            actual = ac.costTo( start.addDays( start.daysInMonth() - start.day() + 1 ) );
                        } else {
                            actual = ac.cost( start, start.daysInMonth() - start.day() + 1);
                        }
                        return formatMoney( planned, actual );
                    }
                    default:
                        return 0.0;
                        break;
                }
            }
        }
    } else if ( role == Qt::ToolTipRole ) {
        switch ( index.column() ) {
            case 0: return a->name();
            case 1: return a->description();
            case 2: {
                double act = m_actualCostMap.value( a ).totalCost();
                double pl = m_plannedCostMap.value( a ).totalCost();
                return i18n( "Actual total cost: %1, planned total cost: %2", m_project->locale()->formatMoney( act, "", 0 ), m_project->locale()->formatMoney( pl, "", 0 ) );
            }
            default: break;
        }
    } else if ( role == Qt::TextAlignmentRole ) {
        return headerData( index.column(), Qt::Horizontal, role );
    }
    return QVariant();
}

int CostBreakdownItemModel::periodType() const
{
    return m_periodtype;
}

void CostBreakdownItemModel::setPeriodType( int period )
{
    if ( m_periodtype != period ) {
        m_periodtype = period;
        reset();
    }
}

int CostBreakdownItemModel::startMode() const
{
    return m_startmode;
}

void CostBreakdownItemModel::setStartMode( int mode )
{
    m_startmode = mode;
    reset();
}

int CostBreakdownItemModel::endMode() const
{
    return m_endmode;
}

void CostBreakdownItemModel::setEndMode( int mode )
{
    m_endmode = mode;
    qDebug()<<"setEndMode:"<<mode;
    reset();
}

QDate CostBreakdownItemModel::startDate() const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return m_start;
    }
    switch ( m_startmode ) {
        case StartMode_Project: {
            QDate d = m_project->startTime( id() ).date();
            if ( m_plannedStart.isValid() && m_plannedStart < d ) {
                d = m_plannedStart;
            }
            if ( m_actualStart.isValid() && m_actualStart < d ) {
                d = m_actualStart;
            }
            return d;
        }
        default: break;
    }
    return m_start;
}


void CostBreakdownItemModel::setStartDate( const QDate &date )
{
    m_start = date;
    reset();
}

QDate CostBreakdownItemModel::endDate() const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return m_end;
    }
    switch ( m_endmode ) {
        case EndMode_Project: {
            QDate d = m_project->endTime( id() ).date();
            if ( m_plannedEnd.isValid() && m_plannedEnd > d ) {
                d = m_plannedEnd;
            }
            if ( m_actualEnd.isValid() && m_actualEnd > d ) {
                d = m_actualEnd;
            }
            return d;
        }
        case EndMode_CurrentDate: return QDate::currentDate();
        default: break;
    }
    return m_end;
}

void CostBreakdownItemModel::setEndDate( const QDate &date )
{
    qDebug()<<"setEndDate:"<<date<<m_endmode;
    m_end = date;
    reset();
}

bool CostBreakdownItemModel::cumulative() const
{
    return m_cumulative;
}

void CostBreakdownItemModel::setCumulative( bool on )
{
    m_cumulative = on;
    reset();
}

int CostBreakdownItemModel::showMode() const
{
    return m_showmode;
}
void CostBreakdownItemModel::setShowMode( int show )
{
    m_showmode = show;
}

QVariant CostBreakdownItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            if ( section == 0 ) {
                return i18n( "Name" );
            }
            if ( section == 1 ) {
                return i18n( "Description" );
            }
            if ( section == 2 ) {
                return i18n( "Total" );
            }
            int col = section - 3;
            switch ( m_periodtype ) {
                case Period_Day: {
                    return startDate().addDays( col ).toString( Qt::ISODate );
                }
                case Period_Week: {
                    return startDate().addDays( ( col ) * 7 ).weekNumber();
                }
                case Period_Month: {
                    int days = startDate().daysInMonth() - startDate().day() + 1;
                    QDate start = startDate();
                    for ( int i = 0; i < col; ++i ) {
                        start = start.addDays( days );
                        days = start.daysInMonth();
                    }
                    return QDate::shortMonthName( start.month() );
                }
                default:
                    return section;
                    break;
            }
            return QVariant();
        }
        if ( role == Qt::ToolTipRole ) {
            switch ( section ) {
                case 0: return ToolTip::accountName();
                case 1: return ToolTip::accountDescription();
                case 2: return i18n( "The total cost for the account shown as: Actual cost [ Planned cost ]" );
                default: return QVariant();
            }
        }
        if ( role == Qt::TextAlignmentRole ) {
            switch ( section ) {
                case 0: return QVariant();
                case 1: return QVariant();
                default: return Qt::AlignRight;
            }
            return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

Account *CostBreakdownItemModel::account( const QModelIndex &index ) const
{
    return static_cast<Account*>( index.internalPointer() );
}

void CostBreakdownItemModel::slotAccountChanged( Account *account )
{
    fetchData();
    foreach ( Account *a, m_plannedCostMap.keys() ) {
        QModelIndex idx1 = index( a );
        QModelIndex idx2 = index( idx1.row(), columnCount() - 1, parent( idx1 ) );
        //kDebug()<<a->name()<<idx1<<idx2;
        emit dataChanged( idx1, idx2  );
    }
}


} // namespace KPlato

#include "kptaccountsmodel.moc"
