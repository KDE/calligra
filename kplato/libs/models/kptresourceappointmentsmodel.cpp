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
 * Boston, MA 02110-1301, USA.
*/

#include "kptresourceappointmentsmodel.h"

#include "kptglobal.h"
#include "kptappointment.h"
#include "kptcommand.h"
#include "kpteffortcostmap.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"

#include <QDate>
#include <QMimeData>
#include <QList>
#include <QObject>
#include <QStringList>


#include <kglobal.h>
#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include "kdganttglobal.h"

#include <kdebug.h>

namespace KPlato
{

ResourceAppointmentsItemModel::ResourceAppointmentsItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_group( 0 ),
    m_resource( 0 ),
    m_manager( 0 ),
    m_showInternal( true ),
    m_showExternal( true )
{
}

ResourceAppointmentsItemModel::~ResourceAppointmentsItemModel()
{
}

void ResourceAppointmentsItemModel::slotResourceToBeInserted( const ResourceGroup *group, int row )
{
    kDebug()<<group->name()<<row;
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    QModelIndex i = index( group );
    beginInsertRows( i, row, row );
}

void ResourceAppointmentsItemModel::slotResourceInserted( const Resource *r )
{
    kDebug()<<r->name();
    Q_ASSERT( r->parentGroup() == m_group );
    endInsertRows();
    m_group = 0;
    refresh();
    connect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
    connect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
    connect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
    connect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
    connect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
}

void ResourceAppointmentsItemModel::slotResourceToBeRemoved( const Resource *r )
{
    kDebug()<<r->name();
    int row = r->parentGroup()->indexOf( r );
    beginRemoveRows( index( r->parentGroup() ), row, row );
    disconnect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
    disconnect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
    disconnect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
    disconnect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
    disconnect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );

}

void ResourceAppointmentsItemModel::slotResourceRemoved( const Resource *resource )
{
    //kDebug()<<resource->name();
    endRemoveRows();
    refresh();
}

void ResourceAppointmentsItemModel::slotResourceGroupToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug()<<group->name()<<endl;
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    beginInsertRows( QModelIndex(), row, row );
}

void ResourceAppointmentsItemModel::slotResourceGroupInserted( const ResourceGroup *group )
{
    //kDebug()<<group->name()<<endl;
    Q_ASSERT( group == m_group );
    endInsertRows();
    m_group = 0;
}

void ResourceAppointmentsItemModel::slotResourceGroupToBeRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name()<<endl;
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    int row = index( group ).row();
    beginRemoveRows( QModelIndex(), row, row );
}

void ResourceAppointmentsItemModel::slotResourceGroupRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name()<<endl;
    Q_ASSERT( group == m_group );
    endRemoveRows();
    m_group = 0;
}

void ResourceAppointmentsItemModel::slotAppointmentToBeInserted( Resource *r, int row )
{
}

void ResourceAppointmentsItemModel::slotAppointmentInserted( Resource *r, Appointment *a )
{
    refreshData();
    reset();
}

void ResourceAppointmentsItemModel::slotAppointmentToBeRemoved( Resource *r, int row )
{
}

void ResourceAppointmentsItemModel::slotAppointmentRemoved()
{
    refreshData();
    reset();
}

void ResourceAppointmentsItemModel::slotAppointmentChanged( Resource *r, Appointment *a )
{
    int row = rowNumber( r, a );
    Q_ASSERT( row >= 0 );
    refreshData();
    emit dataChanged( createExternalAppointmentIndex( row, 0, a ), createExternalAppointmentIndex( row, columnCount() - 1, a ) );
}

void ResourceAppointmentsItemModel::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm == m_manager ) {
        setScheduleManager( sm );
    }
}

int ResourceAppointmentsItemModel::rowNumber( Resource *res, Appointment *a ) const
{
    int r = 0;
    if ( m_showInternal ) {
        r = res->appointments( id() ).indexOf( a );
        if ( r > -1 ) {
            return r;
        }
        r = res->numAppointments();
    }
    if ( m_showExternal ) {
        int rr = res->externalAppointmentList().indexOf( a );
        if ( rr > -1 ) {
            return r + rr;
        }
    }
    return -1;
}

void ResourceAppointmentsItemModel::setShowInternalAppointments( bool show )
{
    if ( m_showInternal == show ) {
        return;
    }
    m_showInternal = show;
    refreshData();
    reset();
}

void ResourceAppointmentsItemModel::setShowExternalAppointments( bool show )
{
    if ( m_showExternal == show ) {
        return;
    }
    m_showExternal = show;
    refreshData();
    reset();
}

void ResourceAppointmentsItemModel::setProject( Project *project )
{
    kDebug();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        disconnect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( defaultCalendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
      
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        
	disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT(  slotProjectCalculated( ScheduleManager* ) ) );
    
        foreach ( Resource *r, m_project->resourceList() ) {
            disconnect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
            disconnect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
            disconnect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
            disconnect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
            disconnect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
        }
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        connect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
      
        connect( m_project, SIGNAL( defaultCalendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
      
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    
        foreach ( Resource *r, m_project->resourceList() ) {
            connect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
            connect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
            connect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
            connect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
            connect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
        }
    }
    refreshData();
    reset();
    emit refreshed();
}

QDate ResourceAppointmentsItemModel::startDate() const
{
    if ( m_project && m_manager ) {
        return m_project->startTime( id() ).date();
    }
    return QDate::currentDate();
}

QDate ResourceAppointmentsItemModel::endDate() const
{
    if ( m_project && m_manager ) {
        return m_project->endTime( id() ).date();
    }
    return QDate::currentDate();
}

void ResourceAppointmentsItemModel::setScheduleManager( ScheduleManager *sm )
{
    kDebug()<<sm;
    m_manager = sm;
    refreshData();
    reset();
    emit refreshed();
}

long ResourceAppointmentsItemModel::id() const
{
    return m_manager == 0 ? -1 : m_manager->id();
}

Qt::ItemFlags ResourceAppointmentsItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    return flags &= ~Qt::ItemIsEditable;
}


QModelIndex ResourceAppointmentsItemModel::parent( const QModelIndex &idx ) const
{
    if ( !idx.isValid() || m_project == 0 || m_manager == 0 ) {
        kWarning()<<"No data "<<idx;
        return QModelIndex();
    }

    QModelIndex p;
    if ( ! p.isValid() ) {
        Resource *r = resource( idx );
        if ( r ) {
            int row = m_project->indexOf( r->parentGroup() );
            p = createGroupIndex( row, 0, r->parentGroup() );
            //kDebug()<<"Parent:"<<p<<r->parentGroup()->name();
            Q_ASSERT( p.isValid() );
        }
    }
    if ( ! p.isValid() && m_showInternal ) {
        Appointment *a = appointment( idx );
        if ( a && a->resource() && a->resource()->resource() ) {
            Resource *r = a->resource()->resource();
            int row = r->parentGroup()->indexOf( r );
            p = createResourceIndex( row, 0, r );
            //kDebug()<<"Parent:"<<p<<r->name();
            Q_ASSERT( p.isValid() );
        }
    }
    if ( ! p.isValid() && m_showExternal ) {
        Appointment *a = externalAppointment( idx );
        Resource *r = parent( a );
        if ( r ) {
            int row = r->parentGroup()->indexOf( r );
            p = createResourceIndex( row, 0, r );
        }
    }
    if ( ! p.isValid() ) {
        //kDebug()<<"Parent:"<<p;
    }
    //kDebug()<<"Child :"<<idx;
    return p;
}

Resource *ResourceAppointmentsItemModel::parent( const Appointment *a ) const
{
    if ( a == 0 || m_project == 0 ) {
        return 0;
    }
    foreach ( Resource *r, m_project->resourceList() ) {
        if ( r->appointments( id() ).contains( const_cast<Appointment*>( a ) ) ) {
            return r;
        }
        if ( r->externalAppointmentList().contains( const_cast<Appointment*>( a ) ) ) {
            return r;
        }
    }
    return 0;
}

QModelIndex ResourceAppointmentsItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row < m_project->numResourceGroups() ) {
            //kDebug()<<"Group: "<<m_project->resourceGroupAt( row )<<endl;
            return createGroupIndex( row, column, m_project->resourceGroupAt( row ) );
        }
        return QModelIndex();
    }
    ResourceGroup *g = resourcegroup( parent );
    if ( g ) {
        if ( row < g->numResources() ) {
            //kDebug()<<"Resource: "<<g->resourceAt( row )<<endl;
            return createResourceIndex( row, column, g->resourceAt( row ) );
        }
        return QModelIndex();
    }
    Resource *r = resource( parent );
    if ( r && ( m_showInternal || m_showExternal ) ) {
        int num = m_showInternal ? r->numAppointments( id() ) : 0;
        if ( row < num ) {
            //kDebug()<<"Appointment: "<<r->appointmentAt( row, m_manager->id() );
            return createAppointmentIndex( row, column, r->appointmentAt( row, id() ) );
        }
        int extRow = row - num;
        //kDebug()<<"Appointment: "<<r->externalAppointmentList().value( extRow );
        Q_ASSERT( extRow >= 0 && extRow < r->externalAppointmentList().count() );
        return createExternalAppointmentIndex( row, column, r->externalAppointmentList().value( extRow ) );
    }
    return QModelIndex();
}

QModelIndex ResourceAppointmentsItemModel::index( const Resource *resource ) const
{
    if ( m_project == 0 || resource == 0 ) {
        return QModelIndex();
    }
    Resource *r = const_cast<Resource*>(resource);
    int row = -1;
    ResourceGroup *par = r->parentGroup();
    if ( par ) {
        row = par->indexOf( r );
        return createResourceIndex( row, 0, r );
    }
    return QModelIndex();
}

QModelIndex ResourceAppointmentsItemModel::index( const ResourceGroup *group ) const
{
    if ( m_project == 0 || group == 0 ) {
        return QModelIndex();
    }
    ResourceGroup *g = const_cast<ResourceGroup*>(group);
    int row = m_project->indexOf( g );
    return createGroupIndex( row, 0, g );

}

void ResourceAppointmentsItemModel::refresh()
{
    refreshData();
    emit refreshed();
}

void ResourceAppointmentsItemModel::refreshData()
{
    long id = m_manager == 0 ? -1 : m_manager->id();
    //kDebug()<<"Schedule id: "<<id<<endl;
    QDate start;
    QDate end;
    QMap<const Appointment*, EffortCostMap> ec;
    QMap<const Appointment*, EffortCostMap> extEff;
    foreach ( Resource *r, m_project->resourceList() ) {
        foreach (Appointment* a, r->appointments( id )) {
            QDate s = a->startTime().date();
            QDate e = a->endTime().date();
            ec[ a ] = a->plannedPrDay( s, e );
            if ( ! start.isValid() || s < start ) {
                start = s;
            }
            if ( ! end.isValid() || e > end ) {
                end = e;
            }
            //kDebug()<<a->node()->node()->name()<<": "<<s<<e<<": "<<m_effortMap[ a ].totalEffort().toDouble(Duration::Unit_h);
        }
        // add external appointments
        foreach (Appointment* a, r->externalAppointmentList() ) {
            extEff[ a ] = a->plannedPrDay( startDate(), endDate() );
            //kDebug()<<r->name()<<a->auxcilliaryInfo()<<": "<<extEff[ a ].totalEffort().toDouble(Duration::Unit_h);
            //kDebug()<<r->name()<<a->auxcilliaryInfo()<<": "<<extEff[ a ].startDate()<<extEff[ a ].endDate();
        }
    }
    m_effortMap.clear();
    m_externalEffortMap.clear();
    m_effortMap = ec;
    m_externalEffortMap = extEff;
    return;
}

int ResourceAppointmentsItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 3 + startDate().daysTo( endDate() );
}

int ResourceAppointmentsItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    //kDebug()<<parent.row()<<", "<<parent.column()<<endl;
    if ( ! parent.isValid() ) {
        //kDebug()<<m_project->name()<<": "<<m_project->numResourceGroups()<<endl;
        return m_project->numResourceGroups();
    }
    ResourceGroup *g = resourcegroup( parent );
    if ( g ) {
        //kDebug()<<g->name()<<": "<<g->numResources()<<endl;
        return g->numResources();
    }
    Resource *r = resource( parent );
    if ( r ) {
        int rows = m_showInternal ? r->numAppointments( id() ) : 0;
        rows += m_showExternal ? r->numExternalAppointments() : 0;
        return rows;
    }
    return 0;
}

QVariant ResourceAppointmentsItemModel::name( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->name();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::name( const  ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->name();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::name( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->name();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::name( const Appointment *app, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return app->auxcilliaryInfo();
        case Qt::ToolTipRole:
            return i18n( "External project: %1", app->auxcilliaryInfo() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::ForegroundRole:
            if ( m_externalEffortMap.contains( app ) ) {
                return QVariant( Qt::blue );
            }
            break;
    }
    return QVariant();
}


QVariant ResourceAppointmentsItemModel::total( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d;
            if ( m_showInternal ) {
                QList<Appointment*> lst = res->appointments( m_manager->id() );
                foreach ( Appointment *a, lst ) {
                    if ( m_effortMap.contains( a ) ) {
                        d += m_effortMap[ a ].totalEffort();
                    }
                }
            }
            if ( m_showExternal ) {
                QList<Appointment*> lst = res->externalAppointmentList();
                foreach ( Appointment *a, lst ) {
                    if ( m_externalEffortMap.contains( a ) ) {
                        d += m_externalEffortMap[ a ].totalEffort();
                    }
                }
            }
            return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
        }
        case Qt::EditRole:
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignRight;
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::total( const Resource *res, const QDate &date, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d;
            if ( m_showInternal ) {
                QList<Appointment*> lst = res->appointments( id() );
                foreach ( Appointment *a, lst ) {
                    if ( m_effortMap.contains( a ) ) {
                        d += m_effortMap[ a ].effortOnDate( date );
                    }
                }
            }
            if ( m_showExternal ) {
                QList<Appointment*> lst = res->externalAppointmentList();
                foreach ( Appointment *a, lst ) {
                    if ( m_externalEffortMap.contains( a ) ) {
                        d += m_externalEffortMap[ a ].effortOnDate( date );
                    }
                }
            }
            QString ds = KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
            Duration avail = res->effort( 0, DateTime( date, QTime(0,0,0) ), Duration( 1.0, Duration::Unit_d ) );
            QString avails = KGlobal::locale()->formatNumber( avail.toDouble( Duration::Unit_h ), 1 );
            return QString( "%1(%2)").arg( ds).arg( avails );
        }
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return i18n( "The total booking on %1, along with the maximum hours for the resource", KGlobal::locale()->formatDate( date ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignRight;
        case Qt::BackgroundRole: {
            if ( res->calendar() && res->calendar()->state( date ) != CalendarDay::Working ) {
                QColor c( 0xf0f0f0 );
                return QVariant::fromValue( c );
                //return QVariant( Qt::cyan );
            }
            break;
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::total( const Appointment *a, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d;
            if ( m_effortMap.contains( a ) ) {
                d = m_effortMap[ a ].totalEffort();
            } else if ( m_externalEffortMap.contains( a ) ) {
                d = m_externalEffortMap[ a ].totalEffort();
            }
            return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
        }
        case Qt::ToolTipRole: {
            if ( m_effortMap.contains( a ) ) {
                return i18n( "Total booking by this task" );
            } else if ( m_externalEffortMap.contains( a ) ) {
                return i18n( "Total booking by the external project" );
            }
            return QVariant();
        }
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignRight;
        case Qt::ForegroundRole:
            if ( m_externalEffortMap.contains( a ) ) {
                return QVariant( Qt::blue );
            }
            break;
    }
    return QVariant();
}


QVariant ResourceAppointmentsItemModel::assignment( const Appointment *a, const QDate &date, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d;
            if ( m_effortMap.contains( a ) ) {
                if ( date < m_effortMap[ a ].startDate() || date > m_effortMap[ a ].endDate() ) {
                    return QVariant();
                }
                d = m_effortMap[ a ].effortOnDate( date );
                return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
            } else  if ( m_externalEffortMap.contains( a ) ) {
                if ( date < m_externalEffortMap[ a ].startDate() || date > m_externalEffortMap[ a ].endDate() ) {
                    return QVariant();
                }
                d = m_externalEffortMap[ a ].effortOnDate( date );
                return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
            }
            return QVariant();
        }
        case Qt::EditRole:
        case Qt::ToolTipRole: {
            if ( m_effortMap.contains( a ) ) {
                return i18n( "Booking by this task on %1", KGlobal::locale()->formatDate( date ) );
            } else if ( m_externalEffortMap.contains( a ) ) {
                return i18n( "Booking by external project on %1",KGlobal::locale()->formatDate( date ) );
            }
            return QVariant();
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignRight;
        case Qt::ForegroundRole:
            if ( m_externalEffortMap.contains( a ) ) {
                return QVariant( Qt::blue );
            }
            break;
        case Qt::BackgroundRole: {
            Resource *r = parent( a );
            if ( r && r->calendar() && r->calendar()->state( date ) != CalendarDay::Working ) {
                QColor c( 0xf0f0f0 );
                return QVariant::fromValue( c );
                //return QVariant( Qt::cyan );
            }
            break;
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::notUsed( const ResourceGroup *, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return QString(" ");
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::EditRole:
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAppointmentsItemModel::data( const QModelIndex &index, int role ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return QVariant();
    }
    QVariant result;
    if ( index.column() >= columnCount() ) {
        kDebug()<<"invalid display value column "<<index;
        return result;
    }
    if ( ! index.isValid() ) {
        kDebug()<<"Invalid index:"<<index;
        return result;
    }
    Resource *r = resource( index );
    if ( r ) {
        switch ( index.column() ) {
            case 0: result = name( r, role ); break;
            case 1: result = total( r, role ); break;
            default:
                QDate d = startDate().addDays( index.column() - 2 );
                result = total( r, d, role );
                break;
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
    ResourceGroup *g = resourcegroup( index );
    if ( g ) {
        switch ( index.column() ) {
            case 0: result = name( g, role ); break;
            default:
                result = notUsed( g, role );
                break;
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
    Appointment *a = appointment( index );
    if ( a ) {
        switch ( index.column() ) {
            case 0: result = name( a->node()->node(), role ); break;
            case 1: result = total( a, role ); break;
            default: {
                QDate d = startDate().addDays( index.column()-2 );
                result = assignment( a, d, role );
                break;
            }
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
    a = externalAppointment( index );
    if ( a ) {
        //kDebug()<<"external"<<a->auxcilliaryInfo()<<index;
        switch ( index.column() ) {
            case 0: result = name( a, role ); break;
            case 1: result = total( a, role ); break;
            default: {
                QDate d = startDate().addDays( index.column()-2 );
                result = assignment( a, d, role );
                break;
            }
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
    kDebug()<<"Could not find ptr:"<<index;
    return QVariant();
}

bool ResourceAppointmentsItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags( index ) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Resource *r = resource( index );
    if ( r ) {
        switch (index.column()) {
            default:
                qWarning("data: invalid display value column %d", index.column());
                break;
        }
        return false;
    }
    ResourceGroup *g = resourcegroup( index );
    if ( g ) {
        switch (index.column()) {
            default:
                qWarning("data: invalid display value column %d", index.column());
                break;
        }
        return false;
    }
    return false;
}

QVariant ResourceAppointmentsItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Total" );
                default: {
                    //kDebug()<<section<<", "<<startDate()<<endDate();
                    if ( section < columnCount() ) {
                        QDate d = startDate().addDays( section - 2 );
                        if ( d <= endDate() ) {
                            return d;
                        }
                    }
                    return QVariant();
                }
            }
        } else if ( role == Qt::ToolTipRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "The total hours booked" );
                default: {
                    //kDebug()<<section<<", "<<startDate()<<endDate();
                    QDate d = startDate().addDays( section - 2 );
                    return i18n( "Bookings on %1", KGlobal::locale()->formatDate( d ) );
                }
                return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 0: return QVariant();
                default: return Qt::AlignRight;
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

QObject *ResourceAppointmentsItemModel::object( const QModelIndex &index ) const
{
    QObject *o = 0;
    if ( index.isValid() ) {
        o = dynamic_cast<QObject*>( resource( index ) );
        if ( o ) {
            return o;
        }
        o = dynamic_cast<QObject*>( resourcegroup( index ) );
    }
    return o;
}

Node *ResourceAppointmentsItemModel::node( const QModelIndex &index ) const
{
    Appointment *a = appointment( index );
    if ( a == 0 ) {
        return 0;
    }
    return a->node()->node();
}

Appointment *ResourceAppointmentsItemModel::appointment( const QModelIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    foreach ( Resource *r, m_project->resourceList() ) {
        foreach ( Appointment *a, r->appointments( id() ) ) {
            if ( a == index.internalPointer() ) {
                return a;
            }
        }
    }
    return 0;
}

Appointment *ResourceAppointmentsItemModel::externalAppointment( const QModelIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    foreach ( Resource *r, m_project->resourceList() ) {
        foreach ( Appointment *a, r->externalAppointmentList() ) {
            if ( a == index.internalPointer() ) {
                return a;
            }
        }
    }
    return 0;
}

QModelIndex ResourceAppointmentsItemModel::createAppointmentIndex( int row, int col, void *ptr ) const
{
    return createIndex( row, col, ptr );
}

QModelIndex ResourceAppointmentsItemModel::createExternalAppointmentIndex( int row, int col, void *ptr ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return QModelIndex();
    }
    QModelIndex i = createIndex( row, col, ptr );
    //kDebug()<<i;
    return i;
}

Resource *ResourceAppointmentsItemModel::resource( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    foreach ( Resource *r, m_project->resourceList() ) {
        if ( r == index.internalPointer() ) {
            return r;
        }
    }
    return 0;
}

QModelIndex ResourceAppointmentsItemModel::createResourceIndex( int row, int col, void *ptr ) const
{
    return createIndex( row, col, ptr );
}

ResourceGroup *ResourceAppointmentsItemModel::resourcegroup( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    foreach ( ResourceGroup *r, m_project->resourceGroups() ) {
        if ( r == index.internalPointer() ) {
            return r;
        }
    }
    return 0;
}

QModelIndex ResourceAppointmentsItemModel::createGroupIndex( int row, int col, void *ptr ) const
{
    return createIndex( row, col, ptr );
}

void ResourceAppointmentsItemModel::slotCalendarChanged( Calendar* )
{
    foreach ( Resource *r, m_project->resourceList() ) {
        if ( r->calendar( true ) == 0 ) {
            slotResourceChanged( r );
        }
    }
}

void ResourceAppointmentsItemModel::slotResourceChanged( Resource *res )
{
    ResourceGroup *g = res->parentGroup();
    if ( g ) {
        int row = g->indexOf( res );
        emit dataChanged( createResourceIndex( row, 0, res ), createResourceIndex( row, columnCount() - 1, res ) );
        return;
    }
}

void ResourceAppointmentsItemModel::slotResourceGroupChanged( ResourceGroup *res )
{
    Project *p = res->project();
    if ( p ) {
        int row = p->resourceGroups().indexOf( res );
        emit dataChanged( createGroupIndex( row, 0, res ), createGroupIndex( row, columnCount() - 1, res ) );
    }
}

Qt::DropActions ResourceAppointmentsItemModel::supportedDropActions() const
{
    return Qt::CopyAction;
}


QStringList ResourceAppointmentsItemModel::mimeTypes() const
{
    return QStringList() << "text/x-vcard";
}

bool ResourceAppointmentsItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent )
{
    kDebug()<<row<<" p:"<<parent.row()<<endl;
    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0) {
        return false;
    }
    //kDebug()<<row<<" p:"<<parent.row()<<endl;
    ResourceGroup *g = 0;
    if ( parent.isValid() ) {
        g = resourcegroup( parent );
    } else {
        g = resourcegroup( index( row, column, parent ) );
    }
    if ( g == 0 ) {
        return false;
    }
    //kDebug()<<data->formats()<<endl;
    MacroCommand *m = 0;
    if ( data->hasFormat( "text/x-vcard" ) ) {
        QByteArray vcard = data->data( "text/x-vcard" );
        KABC::VCardConverter vc;
        KABC::Addressee::List lst = vc.parseVCards( vcard );
        foreach( const KABC::Addressee &a, lst ) {
            if ( m == 0 ) m = new MacroCommand( i18np( "Add resource from addressbook", "Add %1 resources from addressbook", lst.count() ) );
            Resource *r = new Resource();
            r->setName( a.formattedName() );
            r->setEmail( a.preferredEmail() );
            m->addCommand( new AddResourceCmd( g, r ) );
        }
    }
    if ( m ) {
        emit executeCommand( m );
    }
    return true;
}

//-------------------------------------------------------
class ResourceAppointmentsRowModel::Private
{
public:
    enum Type { None, Group, Resource, Appointment, Interval };
    Private( void *par=0, void *p=0, Type t=None ) : parent( par ), ptr( p ), type( t ) {}

    void *parent;
    void *ptr;
    Type type;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug dbg, ResourceAppointmentsRowModel::Private::Type t)
{
  switch(t){
  case ResourceAppointmentsRowModel::Private::None:       dbg << "Node"; break;
  case ResourceAppointmentsRowModel::Private::Group:      dbg << "Group"; break;
  case ResourceAppointmentsRowModel::Private::Resource:   dbg << "Resource"; break;
  case ResourceAppointmentsRowModel::Private::Appointment:dbg << "Appointment"; break;
  case ResourceAppointmentsRowModel::Private::Interval:   dbg << "Interval"; break;
  default: dbg << "Unknown";
  }
  return dbg;
}
QDebug operator<<( QDebug dbg, const ResourceAppointmentsRowModel::Private& s )
{
    dbg <<&s;
    return dbg;
}
QDebug operator<<( QDebug dbg, const ResourceAppointmentsRowModel::Private* s )
{
    if ( s == 0 ) {
        dbg<<"ResourceAppointmentsRowModel::Private[ ("<<(void*)s<<") ]";
    } else {
        dbg << "ResourceAppointmentsRowModel::Private[ ("<<(void*)s<<") Type="<<s->type<<" parent=";
        switch( s->type ) {
        case ResourceAppointmentsRowModel::Private::Group:
            dbg<<static_cast<Project*>(s->parent)<<static_cast<Project*>(s->parent)->name();
            dbg<<" ptr="<<static_cast<ResourceGroup*>(s->ptr)<<static_cast<ResourceGroup*>(s->ptr)->name();
            break;
        case ResourceAppointmentsRowModel::Private::Resource:
            dbg<<static_cast<ResourceGroup*>(s->parent)<<static_cast<ResourceGroup*>(s->parent)->name();
            dbg<<" ptr="<<static_cast<Resource*>(s->ptr)<<static_cast<Resource*>(s->ptr)->name();
            break;
        case ResourceAppointmentsRowModel::Private::Appointment:
            dbg<<static_cast<Resource*>(s->parent)<<static_cast<Resource*>(s->parent)->name();
            dbg<<" ptr="<<static_cast<Appointment*>(s->ptr);
            break;
        case ResourceAppointmentsRowModel::Private::Interval:
            dbg<<static_cast<Appointment*>(s->parent)<<" ptr="<<static_cast<AppointmentInterval*>(s->ptr);
            break;
        default:
            dbg<<s->parent<<" ptr="<<s->ptr;
            break;
        }
        dbg<<" ]";
    }

    return dbg;
}

#endif

ResourceAppointmentsRowModel::ResourceAppointmentsRowModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 )
{
}

ResourceAppointmentsRowModel::~ResourceAppointmentsRowModel()
{
    qDeleteAll( m_datamap );
}

void ResourceAppointmentsRowModel::setProject( Project *project )
{
    //kDebug()<<project;
    if ( m_project ) {
        disconnect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        disconnect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( defaultCalendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
      
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT(  slotProjectCalculated( ScheduleManager* ) ) );
    
        foreach ( Resource *r, m_project->resourceList() ) {
            disconnect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
            disconnect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
            disconnect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
            disconnect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
            disconnect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
        }
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        connect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
      
        connect( m_project, SIGNAL( defaultCalendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
      
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    
        foreach ( Resource *r, m_project->resourceList() ) {
            connect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
            connect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
            connect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
            connect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
            connect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
        }
    }
    reset();
}

void ResourceAppointmentsRowModel::setScheduleManager( ScheduleManager *sm )
{
    //kDebug()<<sm;
    m_manager = sm;
    qDeleteAll( m_datamap );
    m_datamap.clear();
    reset();
}

long ResourceAppointmentsRowModel::id() const
{
    return m_manager ? m_manager->id() : -1;
}

int ResourceAppointmentsRowModel::columnCount( const QModelIndex & parent ) const
{
    return 5;
}

int ResourceAppointmentsRowModel::rowCount( const QModelIndex & parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( ! parent.isValid() ) {
        return m_project->numResourceGroups();
    }
    if ( ResourceGroup *g = resourcegroup( parent ) ) {
        return g->numResources();
    }
    if ( m_manager == 0 ) {
        return 0;
    }
    if ( Resource *r = resource( parent ) ) {
        return r->numAppointments( id() ); // number of tasks there are appointments with
    }
    if ( Appointment *a = appointment( parent ) ) {
        return a->count(); // number of appointment intervals
    }
    return 0;
}

QVariant ResourceAppointmentsRowModel::data( const ResourceGroup *g, int column, int role ) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( column ) {
            case 0: return g->name();
            case 1: return g->typeToString( true );
            case 2: return " ";
            case 3: return " ";
            case 4: return " ";
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsRowModel::data( const Resource *r, int column, int role ) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( column ) {
            case 0: return r->name();
            case 1: return r->typeToString( true );
            case 2: return " ";
            case 3: return " ";
            case 4: return " ";
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsRowModel::data( const Appointment *a, int column, int role ) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( column ) {
            case 0: return a->node()->node()->name();
            case 1: return a->node()->node()->typeToString( true );
            case 2: return a->startTime().dateTime();
            case 3: return a->endTime().dateTime();
            case 4: return " ";
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsRowModel::data( const AppointmentInterval *a, int column, int role ) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( column ) {
            case 0: return QVariant();
            case 1: return i18n( "Interval" );
            case 2: return a->startTime().dateTime();
            case 3: return a->endTime().dateTime();
            case 4: return a->load();
        }
    }
    return QVariant();
}

QVariant ResourceAppointmentsRowModel::data( const QModelIndex &index, int role ) const
{
    //kDebug()<<index<<role;

    if ( m_project == 0 || ! index.isValid() ) {
        return QVariant();
    }
    if ( ResourceGroup *g = resourcegroup( index ) ) {
        return data( g, index.column(), role );
    }
    if ( Resource *r = resource( index ) ) {
        return data( r, index.column(), role );
    }
    if ( m_manager == 0 ) {
        return QVariant();
    }
    if ( Appointment *a = appointment( index ) ) {
        return data( a, index.column(), role );
    }
    if ( AppointmentInterval *i = interval( index ) ) {
        return data( i, index.column(), role );
    }
    return QVariant();
}

QVariant ResourceAppointmentsRowModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return QVariant();
    }
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case 0: return i18n( "Name" );
            case 1: return i18n( "Type" );
            case 2: return i18n( "Start" );
            case 3: return i18n( "Finish" );
            case 4: return i18n( "Load" );
        }
    }
    return QVariant();
}

QModelIndex ResourceAppointmentsRowModel::parent( const QModelIndex &idx ) const
{
    if ( !idx.isValid() || m_project == 0 ) {
        kWarning()<<"No data "<<idx;
        return QModelIndex();
    }

    QModelIndex p;
    if ( resourcegroup( idx ) ) {
        // ResourceGroup, no parent
        return QModelIndex();
    }
    if ( ResourceGroup *pg = parentGroup( idx ) ) {
        // Resource, parent is ResourceGroup
        int row = m_project->indexOf( pg );
        p = const_cast<ResourceAppointmentsRowModel*>( this )->createGroupIndex( row, 0, m_project );
        //kDebug()<<"Parent:"<<p<<r->parentGroup()->name();
        Q_ASSERT( p.isValid() );
        return p;
    }
    if ( Resource *pr = parentResource( idx ) ) {
        // Appointment, parent is Resource
        int row = pr->parentGroup()->indexOf( pr );
        p = const_cast<ResourceAppointmentsRowModel*>( this )->createResourceIndex( row, 0, pr->parentGroup() );
        //kDebug()<<"Parent:"<<p<<r->parentGroup()->name();
        Q_ASSERT( p.isValid() );
        return p;
    }
    if ( Appointment *a = parentAppointment( idx ) ) {
        // AppointmentInterval, parent is Appointment
        Q_ASSERT( a->resource()->id() == id() );
        if ( a->resource() && a->resource()->resource() ) {
            Resource *r = a->resource()->resource();
            int row = r->indexOf( a, id() );
            Q_ASSERT( row >= 0 );
            p = const_cast<ResourceAppointmentsRowModel*>( this )->createAppointmentIndex( row, 0, r );
            //kDebug()<<"Parent:"<<p<<r->name();
            Q_ASSERT( p.isValid() );
        }
        return p;
    }
    return QModelIndex();
}

QModelIndex ResourceAppointmentsRowModel::index( ResourceGroup *g ) const
{
    if ( m_project == 0 || g == 0 ) {
        return QModelIndex();
    }
    return const_cast<ResourceAppointmentsRowModel*>( this )->createGroupIndex( m_project->indexOf( g ), 0, m_project );
}

QModelIndex ResourceAppointmentsRowModel::index( Resource *r ) const
{
    if ( m_project == 0 || r == 0 ) {
        return QModelIndex();
    }
    return const_cast<ResourceAppointmentsRowModel*>( this )->createResourceIndex( r->parentGroup()->indexOf( r ), 0, r->parentGroup() );
}

QModelIndex ResourceAppointmentsRowModel::index( Appointment *a ) const
{
    if ( m_project == 0 || m_manager == 0 || a == 0 || a->resource()->resource() ) {
        return QModelIndex();
    }
    Resource *r = a->resource()->resource();
    return const_cast<ResourceAppointmentsRowModel*>( this )->createAppointmentIndex( r->indexOf( a, id() ), 0, r );
}

QModelIndex ResourceAppointmentsRowModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || row < 0 || column < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row < m_project->numResourceGroups() ) {
            //kDebug()<<"Group: "<<m_project->resourceGroupAt( row );
            return const_cast<ResourceAppointmentsRowModel*>( this )->createGroupIndex( row, column, m_project );
        }
        return QModelIndex();
    }
    if ( ResourceGroup *g = resourcegroup( parent ) ) {
        if ( row < g->numResources() ) {
            //kDebug()<<"Resource: "<<g->resourceAt( row );
            return const_cast<ResourceAppointmentsRowModel*>( this )->createResourceIndex( row, column, g );
        }
        return QModelIndex();
    }
    if ( m_manager == 0 ) {
        return QModelIndex();
    }
    if ( Resource *r = resource( parent ) ) {
        int num = r->numAppointments( id() );
        if ( row < num ) {
            //kDebug()<<"Appointment: "<<r->appointmentAt( row, m_manager->id() );
            return const_cast<ResourceAppointmentsRowModel*>( this )->createAppointmentIndex( row, column, r );
        }
        return QModelIndex();
    }
    if ( Appointment *a = appointment( parent ) ) {
        int num = a->count();
        if ( row < num ) {
            //kDebug()<<"Appointment: "<<r->appointmentAt( row, m_manager->id() );
            return const_cast<ResourceAppointmentsRowModel*>( this )->createIntervalIndex( row, column, a );
        }
        return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex ResourceAppointmentsRowModel::createGroupIndex( int row, int column, Project *project )
{
    Private *p = 0;
    ResourceGroup *group = project->resourceGroupAt( row );
    if ( m_datamap.contains( group ) ) {
        p = m_datamap[ group ];
    } else {
        p = new Private( m_project, group, Private::Group );
        m_datamap.insert( group, p );
    }
    QModelIndex idx = createIndex( row, column, p );
    Q_ASSERT( idx.isValid() );
    return createIndex( row, column, p );
}

QModelIndex ResourceAppointmentsRowModel::createResourceIndex( int row, int column, ResourceGroup *g )
{
    Private *p = 0;
    Resource *res = g->resourceAt( row );
    if ( m_datamap.contains( res ) ) {
        p = m_datamap[ res ];
    } else {
        p = new Private( g, res, Private::Resource );
        m_datamap.insert( res, p );
        //qDebug()<<"createResourceIndex: new"<<p;
    }
    QModelIndex idx = createIndex( row, column, p );
    Q_ASSERT( idx.isValid() );
    return createIndex( row, column, p );
}

QModelIndex ResourceAppointmentsRowModel::createAppointmentIndex( int row, int column, Resource *r )
{
    Private *p = 0;
    Appointment *a = r->appointmentAt( row, id() );
    Q_ASSERT( a );
    if ( m_datamap.contains( a ) ) {
        p = m_datamap[ a ];
    } else {
        p = new Private( r, a, Private::Appointment );
        m_datamap.insert( a, p );
        //qDebug()<<"createAppointmentIndex: new"<<p;
    }
    QModelIndex idx = createIndex( row, column, p );
    Q_ASSERT( idx.isValid() );
    return idx;
}

QModelIndex ResourceAppointmentsRowModel::createIntervalIndex( int row, int column, Appointment *a )
{
    Private *p = 0;
    AppointmentInterval i = a->intervalAt( row );
    foreach ( Private *pr, m_datamap ) {
        if ( pr->parent == a &&
            pr->type == Private::Interval &&
            i == *( static_cast<AppointmentInterval*>( pr->ptr ) ) )
        {
            p = pr;
            break;
        }
    }
    if ( p == 0 ) {
        p = new Private( a, new AppointmentInterval( i ), Private::Interval );
        m_datamap.insert( p->ptr, p );
        //qDebug()<<"createIntervalIndex: new"<<p;
    }
    QModelIndex idx = createIndex( row, column, p );
    Q_ASSERT( idx.isValid() );
    return createIndex( row, column, p );
}

void ResourceAppointmentsRowModel::slotResourceToBeInserted( const ResourceGroup *group, int row )
{
    kDebug()<<group->name()<<row;
    QModelIndex i = index( const_cast<ResourceGroup*>( group ) );
    beginInsertRows( i, row, row );
}

void ResourceAppointmentsRowModel::slotResourceInserted( const Resource *r )
{
    kDebug()<<r->name();
    endInsertRows();

    connect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
    connect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
    connect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
    connect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
    connect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );
}

void ResourceAppointmentsRowModel::slotResourceToBeRemoved( const Resource *r )
{
    kDebug()<<r->name();
    int row = r->parentGroup()->indexOf( r );

    beginRemoveRows( index( r->parentGroup() ), row, row );
    disconnect( r, SIGNAL( externalAppointmentToBeAdded( Resource*, int ) ), this, SLOT( slotAppointmentToBeInserted( Resource*, int ) ) );
    disconnect( r, SIGNAL( externalAppointmentAdded( Resource*, Appointment* ) ), this, SLOT( slotAppointmentInserted( Resource*, Appointment* ) ) );
    disconnect( r, SIGNAL( externalAppointmentToBeRemoved( Resource*, int ) ), this, SLOT( slotAppointmentToBeRemoved( Resource*, int ) ) );
    disconnect( r, SIGNAL( externalAppointmentRemoved() ), this, SLOT( slotAppointmentRemoved() ) );
    disconnect( r, SIGNAL( externalAppointmentChanged( Resource* , Appointment* ) ), this, SLOT( slotAppointmentChanged( Resource* , Appointment* ) ) );

}

void ResourceAppointmentsRowModel::slotResourceRemoved( const Resource *resource )
{
    //kDebug()<<resource->name();
    if ( m_datamap.contains( const_cast<Resource*>( resource ) ) ) {
        delete m_datamap[ const_cast<Resource*>( resource ) ];
        m_datamap.remove( const_cast<Resource*>( resource ) );
        foreach ( Appointment *a, resource->appointments( id() ) ) {
            QList<Private*> lst;
            foreach ( Private *i, m_datamap ) {
                if ( i->parent == a ) {
                    lst << i;
                }
            }
            while ( ! lst.isEmpty() ) {
                m_datamap.remove( lst.first() );
                delete lst.takeFirst();
            }
            if ( m_datamap.contains( a ) ) {
                delete m_datamap[ a ];
                m_datamap.remove( a );
            }
        }
    }
    endRemoveRows();
}

void ResourceAppointmentsRowModel::slotResourceGroupToBeInserted( const ResourceGroup *group, int row )
{
    beginInsertRows( QModelIndex(), row, row );
}

void ResourceAppointmentsRowModel::slotResourceGroupInserted( const ResourceGroup*group )
{
    endInsertRows();
}

void ResourceAppointmentsRowModel::slotResourceGroupToBeRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name()<<endl;
    int row = m_project->indexOf( const_cast<ResourceGroup*>( group ) );
    beginRemoveRows( QModelIndex(), row, row );
}

void ResourceAppointmentsRowModel::slotResourceGroupRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name()<<endl;
    if ( m_datamap.contains( const_cast<ResourceGroup*>( group ) ) ) {
        delete m_datamap[ const_cast<ResourceGroup*>( group ) ];
        m_datamap.remove( const_cast<ResourceGroup*>( group ) );
    }
    endRemoveRows();
}

void ResourceAppointmentsRowModel::slotAppointmentToBeInserted( Resource *r, int row )
{
    // external appointments only, (Internal handled in slotProjectCalculated)
    //beginInsertRows( index( r ), row, row );
}

void ResourceAppointmentsRowModel::slotAppointmentInserted( Resource *r, Appointment *a )
{
    // external appointments only, (Internal handled in slotProjectCalculated)
    //endInsertRows();
}

void ResourceAppointmentsRowModel::slotAppointmentToBeRemoved( Resource *r, int row )
{
    // external appointments only, (Internal handled in slotProjectCalculated)
    //beginRemoveRows( index( r ), row, row );
}

void ResourceAppointmentsRowModel::slotAppointmentRemoved()
{
    // external appointments only, (Internal handled in slotProjectCalculated)
    //endRemoveRows();
}

void ResourceAppointmentsRowModel::slotAppointmentChanged( Resource *r, Appointment *a )
{
    // external appointments only, (Internal handled in slotProjectCalculated)
}

void ResourceAppointmentsRowModel::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm == m_manager ) {
        setScheduleManager( sm );
    }
}

ResourceGroup *ResourceAppointmentsRowModel::parentGroup( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *ch = static_cast<Private*>( index.internalPointer() );
        if ( ch->type == Private::Resource ) {
            return static_cast<ResourceGroup*>( ch->parent );
        }
    }
    return 0;
}

ResourceGroup *ResourceAppointmentsRowModel::resourcegroup( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *p = static_cast<Private*>( index.internalPointer() );
        if ( p->type == Private::Group ) {
            return static_cast<ResourceGroup*>( p->ptr );
        }
    }
    return 0;
}

Resource *ResourceAppointmentsRowModel::parentResource( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *ch = static_cast<Private*>( index.internalPointer() );
        if ( ch->type == Private::Appointment ) {
            return static_cast<Resource*>( ch->parent );
        }
    }
    return 0;
}


Resource *ResourceAppointmentsRowModel::resource( const QModelIndex &index ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *p = static_cast<Private*>( index.internalPointer() );
        if ( p->type == Private::Resource ) {
            return static_cast<Resource*>( p->ptr );
        }
    }
    return 0;
}

Appointment *ResourceAppointmentsRowModel::parentAppointment( const QModelIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *ch = static_cast<Private*>( index.internalPointer() );
        if ( ch->type == Private::Interval ) {
            return static_cast<Appointment*>( ch->parent );
        }
    }
    return 0;
}

Appointment *ResourceAppointmentsRowModel::appointment( const QModelIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 || ! index.isValid() ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *p = static_cast<Private*>( index.internalPointer() );
        if ( p->type == Private::Appointment ) {
            return static_cast<Appointment*>( p->ptr );
        }
    }
    return 0;
}

AppointmentInterval *ResourceAppointmentsRowModel::interval( const QModelIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( m_datamap.values().contains( static_cast<Private*>( index.internalPointer() ) ) ) {
        Private *p = static_cast<Private*>( index.internalPointer() );
        if ( p->type == Private::Interval ) {
            return static_cast<AppointmentInterval*>( p->ptr );
        }
    }
    return 0;
}

//---------------------------------------------
ResourceAppointmentsGanttModel::ResourceAppointmentsGanttModel( QObject *parent )
    : ResourceAppointmentsRowModel( parent )
{
}

ResourceAppointmentsGanttModel::~ResourceAppointmentsGanttModel()
{
}

QVariant ResourceAppointmentsGanttModel::data( const ResourceGroup *g, int column, int role ) const
{
    if ( role == KDGantt::ItemTypeRole ) {
        return KDGantt::TypeSummary;
    } else if ( role == KDGantt::StartTimeRole ) {
        return g->startTime( id() ).dateTime();
    } else if ( role == KDGantt::EndTimeRole ) {
        return g->endTime( id() ).dateTime();
    }
    return ResourceAppointmentsRowModel::data( g, column, role );
}

QVariant ResourceAppointmentsGanttModel::data( const Resource *r, int column, int role ) const
{
    if ( role == KDGantt::ItemTypeRole ) {
        return KDGantt::TypeSummary;
    } else if ( role == KDGantt::StartTimeRole ) {
        return r->startTime( id() ).dateTime();
    } else if ( role == KDGantt::EndTimeRole ) {
        return r->endTime( id() ).dateTime();
    }
    return ResourceAppointmentsRowModel::data( r, column, role );
}

QVariant ResourceAppointmentsGanttModel::data( const Appointment *a, int column, int role ) const
{
    //qDebug()<<"data app:"<<column<<role;
    if ( role == KDGantt::ItemTypeRole ) {
        return KDGantt::TypeMulti;
    } else if ( role == KDGantt::StartTimeRole ) {
        return a->startTime().dateTime();
    } else if ( role == KDGantt::EndTimeRole ) {
        return a->endTime().dateTime();
    }
    return ResourceAppointmentsRowModel::data( a, column, role );
}

QVariant ResourceAppointmentsGanttModel::data( const AppointmentInterval *a, int column, int role ) const
{
    if ( role == KDGantt::ItemTypeRole ) {
        return KDGantt::TypeTask;
    } else if ( role == KDGantt::StartTimeRole ) {
        return a->startTime().dateTime();
    } else if ( role == KDGantt::EndTimeRole ) {
        return a->endTime().dateTime();
    } else if ( role == KDGantt::TaskCompletionRole ) {
        return 0;
    }
    return ResourceAppointmentsRowModel::data( a, column, role );
}


} // namespace KPlato

#include "kptresourceappointmentsmodel.moc"
