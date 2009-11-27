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
#include "kptcommonstrings.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptdatetime.h"
#include "kptschedulerplugin.h"

#include <QObject>
#include <QStringList>


#include <kglobal.h>
#include <klocale.h>
#include <KIcon>
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
    m_manager( 0 ),
    m_flat( false )
{
}

ScheduleItemModel::~ScheduleItemModel()
{
}

void ScheduleItemModel::slotScheduleManagerToBeInserted( const ScheduleManager *parent, int row )
{
    kDebug()<<parent;
    if ( m_flat ) {
        return; // handle in *Inserted();
    }
    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(parent);
    beginInsertRows( index( parent ), row, row );
}

void ScheduleItemModel::slotScheduleManagerInserted( const ScheduleManager *manager )
{
    if ( m_flat ) {
        int row = m_project->allScheduleManagers().indexOf( const_cast<ScheduleManager*>( manager ) );
        Q_ASSERT( row >= 0 );
        beginInsertRows( QModelIndex(), row, row );
        m_managerlist.insert( row, const_cast<ScheduleManager*>( manager ) );
        endInsertRows();
        emit scheduleManagerAdded( const_cast<ScheduleManager*>( manager ) );
        return;
    }
    Q_ASSERT( manager->parentManager() == m_manager );
    endInsertRows();
    m_manager = 0;
    emit scheduleManagerAdded( const_cast<ScheduleManager*>( manager ) );
}

void ScheduleItemModel::slotScheduleManagerToBeRemoved( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
    if ( m_flat ) {
        int row = m_managerlist.indexOf( const_cast<ScheduleManager*>( manager ) );
        beginRemoveRows( QModelIndex(), row, row );
        m_managerlist.removeAt( row );
        return;
    }

    Q_ASSERT( m_manager == 0 );
    m_manager = const_cast<ScheduleManager*>(manager);
    QModelIndex i = index( manager );
    int row = i.row();
    beginRemoveRows( parent( i ), row, row );
}

void ScheduleItemModel::slotScheduleManagerRemoved( const ScheduleManager *manager )
{
    kDebug()<<manager->name();
    if ( m_flat ) {
        endRemoveRows();
        return;
    }
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
    setFlat( m_flat ); // update m_managerlist
    reset();
}

void ScheduleItemModel::slotManagerChanged( ScheduleManager *sch )
{
    if ( m_flat ) {
        int row = m_managerlist.indexOf( sch );
        emit dataChanged( createIndex( row, 0, sch ), createIndex( row, columnCount(), sch ) );
        return;
    }

    int r = sch->parentManager() ? sch->parentManager()->indexOf( sch ) : m_project->indexOf( sch );
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
    if ( !m_readWrite  ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags &= ~Qt::ItemIsEditable;
    ScheduleManager *sm = manager( index );
    if ( sm && ! sm->isBaselined() ) {
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
    return flags;
}


QModelIndex ScheduleItemModel::parent( const QModelIndex &inx ) const
{
    if ( !inx.isValid() || m_project == 0 || m_flat ) {
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
    if ( m_flat ) {
        return createIndex( row, column, m_managerlist[ row ] );
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
    if ( m_flat ) {
        return createIndex( m_managerlist.indexOf( const_cast<ScheduleManager*>( manager ) ), 0, const_cast<ScheduleManager*>(manager) );
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
    if ( m_flat ) {
        return m_managerlist.count();
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
        case Qt::DecorationRole:
            if ( sm->isBaselined() ) {
                return KIcon( "view-time-schedule-baselined" );
            }
            return QVariant();
        default:
            break;
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

QVariant ScheduleItemModel::scheduler( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->schedulerPluginId();
        case Qt::DisplayRole:
            return sm->schedulerPlugin()->name();
        case Qt::ToolTipRole:
            return sm->schedulerPlugin()->comment();
        case Role::EnumList:
            return sm->schedulerPluginNames();
        case Role::EnumListValue:
            return sm->schedulerPluginIndex();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ScheduleItemModel::setScheduler( const QModelIndex &index, const QVariant &value, int role )
{
    ScheduleManager *sm = manager( index );
    if ( sm != 0 ) {
        switch ( role ) {
            case Qt::EditRole: {
                emit executeCommand(new ModifyScheduleManagerSchedulerCmd( *sm, value.toInt(), "Modify Scheduling Direction" ) );
                return true;
            }
        }
    }
    return false;
}

QVariant ScheduleItemModel::isScheduled( const QModelIndex &index, int role ) const
{
    ScheduleManager *sm = manager( index );
    if ( sm == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::EditRole: 
            return sm->isScheduled();
        case Qt::DisplayRole:
            return sm->isScheduled() ? i18n( "Scheduled" ) : i18n( "Not scheduled" );
        case Qt::ToolTipRole:
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
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
        case ScheduleModel::ScheduleScheduler: result = scheduler( index, role ); break;
        case ScheduleModel::ScheduleScheduled: result = isScheduled( index, role ); break;
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
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
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
        case ScheduleModel::ScheduleScheduler: return setScheduler( index, value, role ); break;
        case ScheduleModel::ScheduleScheduled: break;
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
                case ScheduleModel::ScheduleScheduler: return i18n( "Scheduler" );
                case ScheduleModel::ScheduleScheduled: return i18n( "Scheduled" );
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
            case ScheduleModel::ScheduleScheduler: return ToolTip::scheduleScheduler();
            case ScheduleModel::ScheduleScheduled: return QVariant();
            default: return QVariant();
        }
    } else if ( role == Qt::WhatsThisRole ) {
        switch ( section ) {
            case ScheduleModel::ScheduleDirection: return WhatsThis::schedulingDirection();
            case ScheduleModel::ScheduleOverbooking: return WhatsThis::scheduleOverbooking();
            case ScheduleModel::ScheduleDistribution: return WhatsThis::scheduleDistribution();
            case ScheduleModel::ScheduleScheduler: return WhatsThis::scheduleScheduler();
            default: return QVariant();
        }
    }

    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *ScheduleItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case ScheduleModel::ScheduleDirection: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleOverbooking: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleDistribution: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleCalculate: return new EnumDelegate( parent );
        case ScheduleModel::ScheduleScheduler: return new EnumDelegate( parent );
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

void ScheduleItemModel::setFlat( bool flat )
{
    m_flat = flat;
    m_managerlist.clear();
    if ( ! flat || m_project == 0 ) {
        return;
    }
    m_managerlist = m_project->allScheduleManagers();
}

//--------------------------------------
ScheduleSortFilterModel::ScheduleSortFilterModel( QObject *parent )
    : QSortFilterProxyModel( parent )
{
}

ScheduleSortFilterModel::~ScheduleSortFilterModel()
{
}

ScheduleManager *ScheduleSortFilterModel::manager( const QModelIndex &index ) const
{
    QModelIndex i = mapToSource( index );
    const ScheduleItemModel *m = qobject_cast<const ScheduleItemModel*>( i.model() );
    return m == 0 ? 0 : m->manager( i );
}

//--------------------------------------
ScheduleLogItemModel::ScheduleLogItemModel( QObject *parent )
    : QStandardItemModel( parent ),
    m_project( 0 ),
    m_manager( 0 ),
    m_schedule( 0 )
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
        clear();
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
        clear();
        if ( m_manager ) {
            m_schedule = m_manager->expected();
            refresh();
        }
    }
}

void ScheduleLogItemModel::refresh()
{
    clear();
    QStringList lst;
    lst << i18n( "Name" ) << i18n( "Phase" ) << i18n( "Severity" ) << i18n( "Message" );
    setHorizontalHeaderLabels( lst );
    
    if ( m_schedule == 0 ) {
        kDebug()<<"No main schedule";
        return;
    }
    kDebug()<<m_schedule<<m_schedule->logs().count();
    QStandardItem *parentItem = invisibleRootItem();
    QList<Schedule::Log>::ConstIterator it;
    for ( it = m_schedule->logs().constBegin(); it != m_schedule->logs().constEnd(); ++it ) {
        QList<QStandardItem*> lst;
        if ( (*it).resource ) {
            lst.append( new QStandardItem( (*it).resource->name() ) );
        } else {
            lst.append( new QStandardItem( (*it).node->name() ) );
        }
        lst.append( new QStandardItem( m_schedule->logPhase( (*it).phase ) ) );
        QStandardItem *item = new QStandardItem( m_schedule->logSeverity( (*it).severity ) );
        item->setData( (*it).severity );
        lst.append( item );
        lst.append( new QStandardItem( (*it).message ) );
        parentItem->appendRow( lst );
        //kDebug()<<rowCount()<<columnCount()<<parentItem<<lst;
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
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

} // namespace KPlato

#include "kptschedulemodel.moc"
