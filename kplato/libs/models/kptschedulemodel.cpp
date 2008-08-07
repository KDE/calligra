/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen kplato@kde.org>

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

#include "kptschedulemodel.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptdatetime.h"

#include <QObject>
#include <QStringList>


#include <kglobal.h>
#include <klocale.h>

#include <kdebug.h>

namespace KPlato
{

//--------------------------------------

ScheduleModel::ScheduleModel( QObject *parent )
    : QObject( parent )
{
}

ScheduleModel::~ScheduleModel()
{
}

const QMetaEnum ScheduleModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

int ScheduleModel::propertyCount() const
{
    return columnMap().keyCount();
}

//--------------------------------------

ScheduleItemModel::ScheduleItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 )
{
}

ScheduleItemModel::~ScheduleItemModel()
{
}

void ScheduleItemModel::slotScheduleManagerToBeInserted( const ScheduleManager *parent, int row )
{
    kDebug()<<parent;
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(parent);
    beginInsertRows( index( parent ), row, row );
}

void ScheduleItemModel::slotScheduleManagerInserted( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
    Q_ASSERT( manager->parentManager() == m_manager );
    endInsertRows();
    m_manager = 0;
}

void ScheduleItemModel::slotScheduleManagerToBeRemoved( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(manager);
    QModelIndex i = index( manager );
    int row = i.row();
    beginRemoveRows( parent( i ), row, row );
}

void ScheduleItemModel::slotScheduleManagerRemoved( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
    Q_ASSERT( manager == m_manager );
    endRemoveRows();
    m_manager = 0;
}

void ScheduleItemModel::slotScheduleToBeInserted( const ScheduleManager *, int /*row*/ )
{
}

void ScheduleItemModel::slotScheduleInserted( const MainSchedule * )
{
}

void ScheduleItemModel::slotScheduleToBeRemoved( const MainSchedule * )
{
}

void ScheduleItemModel::slotScheduleRemoved( const MainSchedule * )
{
}

void ScheduleItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleManagerToBeInserted( const ScheduleManager*, int) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleManagerAdded( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerInserted( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        connect( m_project, SIGNAL( scheduleManagerToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleManagerToBeInserted( const ScheduleManager*, int) ) );
        
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerAdded( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerInserted( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerRemoved( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        connect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
    reset();
}

void ScheduleItemModel::slotManagerChanged( ScheduleManager *sch )
{
    int r = m_project->indexOf( sch );
    //kDebug()<<sch<<":"<<r;
    emit dataChanged( createIndex( r, 0, sch ), createIndex( r, columnCount(), sch ) );
}


void ScheduleItemModel::slotScheduleChanged( MainSchedule * )
{
}


Qt::ItemFlags ScheduleItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags &= ~Qt::ItemIsEditable;
    ScheduleManager *sm = manager( index );
    if ( sm ) {
        switch ( index.column() ) {
            case ScheduleModel::ScheduleState: break;
            case ScheduleModel::ScheduleDirection:
                if ( sm->parentManager() == 0 ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case ScheduleModel::SchedulePlannedStart: break;
            case ScheduleModel::SchedulePlannedFinish: break;
            default: flags |= Qt::ItemIsEditable; break;
        }
        return flags;
    }
    flags &= ~Qt::ItemIsSelectable;
    return flags;
}


QModelIndex ScheduleItemModel::parent( const QModelIndex &inx ) const
{
    if ( !inx.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<inx.internalPointer()<<":"<<inx.row()<<","<<inx.column();
    ScheduleManager *sm = manager( inx );
    if ( sm == 0 ) {
        return QModelIndex();
    }
    return index( sm->parentManager() );
}

QModelIndex ScheduleItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    //kDebug()<<m_project<<":"<<row<<","<<column;
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 || row >= rowCount( parent ) ) {
        //kDebug()<<row<<","<<column<<" out of bounce";
        return QModelIndex();
    }
    if ( parent.isValid() ) {
        return createIndex( row, column, manager( parent )->children().value( row ) );
    }
    return createIndex( row, column, m_project->scheduleManagers().value( row ) );
}

QModelIndex ScheduleItemModel::index( const ScheduleManager *manager ) const
{
    if ( m_project == 0 || manager == 0 ) {
        return QModelIndex();
    }
    if ( manager->parentManager() == 0 ) {
        return createIndex( m_project->indexOf( manager ), 0, const_cast<ScheduleManager*>(manager) );
    }
    return createIndex( manager->parentManager()->indexOf( manager ), 0, const_cast<ScheduleManager*>(manager) );
}

int ScheduleItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_model.propertyCount();
}

int ScheduleItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( !parent.isValid() ) {
        return m_project->numScheduleManagers();
    }
    ScheduleManager *sm = manager( parent );
    if ( sm ) {
        //kDebug()<<sm->name()<<","<<sm->children().count();
        return sm->children().count();
    }
    return 0;
}

QVariant ScheduleItemModel::name( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return sm->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setName( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            emit executeCommand(new ModifyScheduleManagerNameCmd( *sm, value.toString(), "Modify Schedule Name" ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::state( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole: 
        {
            QStringList l = sm->state();
            if ( l.isEmpty() ) {
                return "";
            }
            return l.first();
        }
        case Qt::ToolTipRole:
            return sm->state().join(", ");
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setState( const QModelIndex &, const QVariant &, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            return false;
    }
    return false;
}

QVariant ScheduleItemModel::allowOverbooking( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->allowOverbooking();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->allowOverbooking() ? i18n( "Allow" ) : i18n( "Avoid" );
        case Role::EnumList:
            return QStringList() << i18n( "Avoid" ) << i18n( "Allow" );
        case Role::EnumListValue:
            return sm->allowOverbooking() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setAllowOverbooking( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            emit executeCommand(new ModifyScheduleManagerAllowOverbookingCmd( *sm, value.toBool(), "Modify Schedule Allow Overbooking" ) );
            return true;
    }
    return false;
}


QVariant ScheduleItemModel::usePert( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->usePert();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->usePert() ? i18n( "PERT" ) : i18n( "None" );
        case Role::EnumList:
            return QStringList() << i18n( "None" ) << i18n( "PERT" );
        case Role::EnumListValue:
            return sm->usePert() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setUsePert( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            emit executeCommand(new ModifyScheduleManagerDistributionCmd( *sm, value.toBool(), "Modify Schedule Distribution" ) );
            emit slotManagerChanged( static_cast<ScheduleManager*>( sm ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::calculateAll( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->calculateAll();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->calculateAll() ? i18n( "All" ) : i18n( "Expected only" );
        case Role::EnumList:
            return QStringList() << i18n( "Expected only" ) << i18n( "All" );
        case Role::EnumListValue:
            return sm->calculateAll() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setCalculateAll( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            emit executeCommand(new ModifyScheduleManagerCalculateAllCmd( *sm, value.toBool(), "Modify Schedule Calculate" ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::projectStart( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( sm->expected() ) {
                return sm->expected()->start().dateTime();
            }
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ScheduleItemModel::projectEnd( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( sm->expected() ) {
                return sm->expected()->end().dateTime();
            }
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ScheduleItemModel::schedulingDirection( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->schedulingDirection();
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return sm->schedulingDirection() ? i18n( "Backwards" ) : i18n( "Forward" );
        case Role::EnumList:
            return QStringList() << i18n( "Forward" ) << i18n( "Backwards" );
        case Role::EnumListValue:
            return sm->schedulingDirection() ? 1 : 0;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setSchedulingDirection( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager ( index );
    if ( sm == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
            emit executeCommand(new ModifyScheduleManagerSchedulingDirectionCmd( *sm, value.toBool(), "Modify Scheduling Direction" ) );
            emit slotManagerChanged( static_cast<ScheduleManager*>( sm ) );
            return true;
    }
    return false;
}

QVariant ScheduleItemModel::data( const QModelIndex &index, int role ) const
{
    //kDebug()<<index.row()<<","<<index.column();
    QVariant result;
    switch ( index.column() ) {
        case ScheduleModel::ScheduleName: result = name( index, role ); break;
        case ScheduleModel::ScheduleState: result = state( index, role ); break;
        case ScheduleModel::ScheduleDirection: result = schedulingDirection( index, role ); break;
        case ScheduleModel::ScheduleOverbooking: result = allowOverbooking( index, role ); break;
        case ScheduleModel::ScheduleDistribution: result = usePert( index, role ); break;
        case ScheduleModel::ScheduleCalculate: result = calculateAll( index, role ); break;
        case ScheduleModel::SchedulePlannedStart: result = projectStart(  index, role ); break;
        case ScheduleModel::SchedulePlannedFinish: result = projectEnd( index, role ); break;
        default:
            kDebug()<<"data: invalid display value column"<<index.column();;
            return QVariant();
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return QVariant();
}

bool ScheduleItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( !index.isValid() || ( flags( index ) & Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    switch (index.column()) {
        case ScheduleModel::ScheduleName: return setName( index, value, role );
        case ScheduleModel::ScheduleState: return setState( index, value, role );
        case ScheduleModel::ScheduleDirection: return setSchedulingDirection( index, value, role );
        case ScheduleModel::ScheduleOverbooking: return setAllowOverbooking( index, value, role );
        case ScheduleModel::ScheduleDistribution: return setUsePert( index, value, role );
        case ScheduleModel::ScheduleCalculate: return setCalculateAll( index, value, role );
        case ScheduleModel::SchedulePlannedStart: return false;
        case ScheduleModel::SchedulePlannedFinish: return false;
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
    }
    return false;
}

QVariant ScheduleItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case ScheduleModel::ScheduleName: return i18n( "Name" );
                case ScheduleModel::ScheduleState: return i18n( "State" );
                case ScheduleModel::ScheduleDirection: return i18n( "Direction" );
                case ScheduleModel::ScheduleOverbooking: return i18n( "Overbooking" );
                case ScheduleModel::ScheduleDistribution: return i18n( "Distribution" );
                case ScheduleModel::ScheduleCalculate: return i18n( "Calculate" );
                case ScheduleModel::SchedulePlannedStart: return i18n( "Planned Start" );
                case ScheduleModel::SchedulePlannedFinish: return i18n( "Planned Finish" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                default: return Qt::AlignCenter;
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case ScheduleModel::ScheduleName: return ToolTip::scheduleName();
            case ScheduleModel::ScheduleState: return ToolTip::scheduleState();
            case ScheduleModel::ScheduleDirection: return ToolTip::schedulingDirection();
            case ScheduleModel::ScheduleOverbooking: return ToolTip::scheduleOverbooking();
            case ScheduleModel::ScheduleDistribution: return ToolTip::scheduleDistribution();
            case ScheduleModel::ScheduleCalculate: return ToolTip::scheduleCalculate();
            case ScheduleModel::SchedulePlannedStart: return ToolTip::scheduleStart();
            case ScheduleModel::SchedulePlannedFinish: return ToolTip::scheduleFinish();
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QItemDelegate *ScheduleItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case ScheduleModel::ScheduleDirection: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleOverbooking: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleDistribution: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleCalculate: return new EnumDelegate( parent );
    }
    return 0;
}

void ScheduleItemModel::sort( int column, Qt::SortOrder order )
{
}

QMimeData * ScheduleItemModel::mimeData( const QModelIndexList & ) const
{
    return 0;
}

QStringList ScheduleItemModel::mimeTypes () const
{
    return QStringList();
}

ScheduleManager *ScheduleItemModel::manager( const QModelIndex &index ) const
{
    ScheduleManager *o = 0;
    if ( index.isValid() && m_project != 0 && index.internalPointer() != 0 && m_project->isScheduleManager( index.internalPointer() ) ) {
        o = static_cast<ScheduleManager*>( index.internalPointer() );
        Q_ASSERT( o );
    }
    return o;
}

//--------------------------------------

ScheduleLogItemModel::ScheduleLogItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 )
{
}

ScheduleLogItemModel::~ScheduleLogItemModel()
{
}

void ScheduleLogItemModel::slotScheduleManagerToBeRemoved( const ScheduleManager *manager )
{
    if ( m_manager == manager ) {
        setManager( 0 );
    }
}

void ScheduleLogItemModel::slotScheduleManagerRemoved( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
}

void ScheduleLogItemModel::slotScheduleToBeInserted( const ScheduleManager *manager, int row )
{
    if ( m_manager && m_manager->expected() /*== ??*/ ) {
        //TODO
    }
}
//FIXME remove const on MainSchedule
void ScheduleLogItemModel::slotScheduleInserted( const MainSchedule *sch )
{
    kDebug()<<m_schedule<<sch;
    if ( m_manager && m_manager == sch->manager() && sch == m_manager->expected() ) {
        m_schedule = const_cast<MainSchedule*>( sch );
        refresh();
    }
}

void ScheduleLogItemModel::slotScheduleToBeRemoved( const MainSchedule *sch )
{
    kDebug()<<m_schedule<<sch;
    if ( m_schedule == sch ) {
        m_schedule = 0;
        m_standard.clear();
    }
}

void ScheduleLogItemModel::slotScheduleRemoved( const MainSchedule *sch )
{
    kDebug()<<m_schedule<<sch;
}

void ScheduleLogItemModel::setProject( Project *project )
{
    kDebug()<<m_project<<"->"<<project;
    if ( m_project ) {
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerRemoved( const ScheduleManager* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        disconnect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        disconnect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotManagerChanged( ScheduleManager* ) ) );
        
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerRemoved( const ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleChanged( MainSchedule* ) ), this, SLOT( slotScheduleChanged( MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeAdded( const ScheduleManager*, int ) ), this, SLOT( slotScheduleToBeInserted( const ScheduleManager*, int ) ) );
        
        connect( m_project, SIGNAL( scheduleToBeRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleToBeRemoved( const MainSchedule* ) ) );
    
        connect( m_project, SIGNAL( scheduleAdded( const MainSchedule* ) ), this, SLOT( slotScheduleInserted( const MainSchedule* ) ) );
        
        connect( m_project, SIGNAL( scheduleRemoved( const MainSchedule* ) ), this, SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    }
}

void ScheduleLogItemModel::setManager( ScheduleManager *manager )
{
    kDebug()<<m_manager<<"->"<<manager;
    if ( manager != m_manager ) {
        m_manager = manager;
        m_schedule = 0;
        m_standard.clear();
        if ( m_manager ) {
            m_schedule = m_manager->expected();
            refresh();
        }
    }
}

void ScheduleLogItemModel::refresh()
{
    m_standard.clear();
    QStringList lst;
    lst << "Name" << "Phase" << "Severity" <<"Message";
    m_standard.setHorizontalHeaderLabels( lst );
    
    if ( m_schedule == 0 ) {
        kDebug()<<"No main schedule";
        return;
    }
    kDebug()<<m_schedule<<m_schedule->logs().count();
    QStandardItem *parentItem = m_standard.invisibleRootItem();
    QList<Schedule::Log>::ConstIterator it;
    for ( it = m_schedule->logs().constBegin(); it != m_schedule->logs().constEnd(); ++it ) {
        QList<QStandardItem*> lst;
        if ( (*it).resource ) {
            lst.append( new QStandardItem( (*it).resource->name() ) );
        } else {
            lst.append( new QStandardItem( (*it).node->name() ) );
        }
        lst.append( new QStandardItem( m_schedule->logPhase( (*it).phase ) ) );
        lst.append( new QStandardItem( m_schedule->logSeverity( (*it).severity ) ) );
        lst.append( new QStandardItem( (*it).message ) );
        parentItem->appendRow( lst );
        //kDebug()<<m_standard.rowCount()<<m_standard.columnCount()<<parentItem<<lst;
    }
}

void ScheduleLogItemModel::slotManagerChanged( ScheduleManager *manager )
{
    kDebug()<<m_manager<<manager;
    if ( m_manager == manager ) {
        //TODO
        refresh();
    }
}


void ScheduleLogItemModel::slotScheduleChanged( MainSchedule *sch )
{
    kDebug()<<m_schedule<<sch;
    if ( m_schedule == sch ) {
        refresh();
    }
}


Qt::ItemFlags ScheduleLogItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags &= ~Qt::ItemIsEditable;
    return flags;
}


QModelIndex ScheduleLogItemModel::parent( const QModelIndex &inx ) const
{
    return m_standard.parent( inx );
}

QModelIndex ScheduleLogItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    QModelIndex ix;
    if ( ! parent.isValid() )
        ix = createIndex( row, column );// m_standard.index( row, column, parent );
    kDebug()<<parent<<ix;
    return ix;
}


int ScheduleLogItemModel::columnCount( const QModelIndex &parent ) const
{
    int col = 2;
    if ( parent.isValid() ) {
        //col = m_standard.columnCount( parent );
    }
    kDebug()<<parent<<col;
    return col;
}

int ScheduleLogItemModel::rowCount( const QModelIndex &parent ) const
{
//    kDebug()<<parent<<m_standard.rowCount( parent );
    if ( ! parent.isValid() )
        return 2;
    return 0;//m_standard.rowCount( parent );
}

QVariant ScheduleLogItemModel::data( const QModelIndex &index, int role ) const
{
    return QVariant();
}

QVariant ScheduleLogItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Message" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                default: return Qt::AlignCenter;
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QItemDelegate *ScheduleLogItemModel::createDelegate( int column, QWidget *parent ) const
{
    return 0;
}

} // namespace KPlato

#include "kptschedulemodel.moc"
