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

#include <kdebug.h>

namespace KPlato
{

ResourceAppointmentsItemModel::ResourceAppointmentsItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_columnCount( 3 ),
    m_group( 0 ),
    m_resource( 0 ),
    m_manager( 0 )
{
}

ResourceAppointmentsItemModel::~ResourceAppointmentsItemModel()
{
}

void ResourceAppointmentsItemModel::slotResourceToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug()<<group->name()<<", "<<row<<endl;
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    beginInsertRows( index( group ), row, row );
}

void ResourceAppointmentsItemModel::slotResourceInserted( const Resource *resource )
{
    //kDebug()<<resource->name()<<endl;
    Q_ASSERT( resource->parentGroup() == m_group );
    endInsertRows();
    m_group = 0;
    refresh();
}

void ResourceAppointmentsItemModel::slotResourceToBeRemoved( const Resource *resource )
{
    //kDebug()<<resource->name()<<endl;
    Q_ASSERT( m_resource == 0 );
    m_resource = const_cast<Resource*>(resource);
    int row = index( resource ).row();
    beginRemoveRows( index( resource->parentGroup() ), row, row );
}

void ResourceAppointmentsItemModel::slotResourceRemoved( const Resource *resource )
{
    //kDebug()<<resource->name()<<endl;
    Q_ASSERT( resource == m_resource );
    endRemoveRows();
    int i = m_resources.indexOf( resource );
    if ( i != -1 ) {
        m_resources.removeAt( i );
    }
    m_resource = 0;
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
    refresh();
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
    foreach ( Resource *r, const_cast<ResourceGroup*>( group )->resources() ) {
        int i = m_resources.indexOf( r );
        if ( i != -1 ) {
            m_resources.removeAt( i );
        }
    }
    int i = m_groups.indexOf( group );
    if ( i != -1 ) {
        m_groups.removeAt( i );
    }
    endRemoveRows();
    m_group = 0;
}

void ResourceAppointmentsItemModel::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm == m_manager ) {
        setScheduleManager( sm );
    }
}

void ResourceAppointmentsItemModel::setProject( Project *project )
{
    kDebug()<<endl;
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
    }
    refresh();
    kDebug()<<endl;
}

void ResourceAppointmentsItemModel::setScheduleManager( ScheduleManager *sm )
{
    kDebug()<<sm<<endl;
    m_manager = sm;
    refresh();
}

Qt::ItemFlags ResourceAppointmentsItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    return flags &= ~Qt::ItemIsEditable;
}


QModelIndex ResourceAppointmentsItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 || m_manager == 0 ) {
        kWarning()<<"No data "<<index<<endl;
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;

    Resource *r = resource( index );
    if ( r ) {
        int row = m_project->indexOf( r->parentGroup() );
        return createGroupIndex( row, 0, const_cast<ResourceGroup*>( r->parentGroup() ) );
    }
    Appointment *a = appointment( index );
    if ( a && a->resource() && a->resource()->resource() ) {
        Resource *r = a->resource()->resource();
        int row = r->parentGroup()->indexOf( r );
        return createResourceIndex( row, 0, r );
    }
    return QModelIndex();
}

QModelIndex ResourceAppointmentsItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_manager == 0 || column < 0 || column >= columnCount() || row < 0 ) {
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
    if ( r ) {
        if ( row < r->numAppointments( m_manager->id() ) ) {
            //kDebug()<<"Appointment: "<<r->appointmentAt( row, m_manager->id() )<<endl;
            return createAppointmentIndex( row, column, r->appointmentAt( row, m_manager->id() ) );
        }
        return QModelIndex();
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
    long id = m_manager == 0 ? -1 : m_manager->id();
    //kDebug()<<"Schedule id: "<<id<<endl;
    QDate start;
    QDate end;
    QMap<const Appointment*, EffortCostMap> ec;
    foreach ( Resource *r, m_project->resourceList() ) {
        QList<Appointment*> lst = r->appointments( id );
        foreach (Appointment* a, lst) {
            QDate s = a->startTime().date();
            QDate e = a->endTime().date();
            ec[ a ] = a->plannedPrDay( s, e );
            if ( ! start.isValid() || s < start ) {
                start = s;
            }
            if ( ! end.isValid() || e > end ) {
                end = e;
            }
            //kDebug()<<a->node()->node()->name()<<": "<<s<<e<<": "<<m_effortMap[ a ].totalEffort().toDouble(Duration::Unit_h)<<endl;
        }
    }
    int cols = QMAX( 2 + start.daysTo( end ), 3 );
    m_groups.clear();
    m_resources.clear();
    m_appointments.clear();
    m_effortMap = ec;
    m_start = start;
    m_end = end;
    kDebug()<<m_columnCount<<" -> "<<cols<<endl;
    m_columnCount = cols;
    reset();
    emit refreshed();
}

int ResourceAppointmentsItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_columnCount;
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
    long id = m_manager->id();
    if ( r ) {
        return r->numAppointments( id );
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

QVariant ResourceAppointmentsItemModel::total( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            QList<Appointment*> lst = res->appointments( m_manager->id() );
            Duration d;
            foreach ( Appointment *a, lst ) {
                if ( m_effortMap.contains( a ) ) {
                    d += m_effortMap[ a ].totalEffort();
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
            QList<Appointment*> lst = res->appointments( m_manager->id() );
            Duration d;
            foreach ( Appointment *a, lst ) {
                if ( m_effortMap.contains( a ) ) {
                    d += m_effortMap[ a ].effortOnDate( date );
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

QVariant ResourceAppointmentsItemModel::total( const Appointment *a, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d = m_effortMap[ a ].totalEffort();
            return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 );
        }
        case Qt::ToolTipRole:
            return " ";
            break;
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignRight;
    }
    return QVariant();
}


QVariant ResourceAppointmentsItemModel::assignment( const Appointment *a, const QDate &date, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            const EffortCostMap &e = m_effortMap[ a ];
            Duration d = e.effortOnDate( date );
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
    QVariant result;
    if ( index.column() >= m_columnCount ) {
        kDebug()<<"invalid display value column "<<index.column()<<" ("<<columnCount()<<")"<<endl;;
        return result;
    }
    Resource *r = resource( index );
    if ( r ) {
        switch ( index.column() ) {
            case 0: result = name( r, role ); break;
            case 1: result = total( r, role ); break;
            default:
                if ( m_start.isValid() && m_end.isValid() ) {
                    QDate d = m_start.addDays( index.column() - 2 );
                    result = total( r, d, role );
                } else {
                    result = "";
                }
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
            default:
                if ( m_start.isValid() && m_end.isValid() ) {
                    QDate d = m_start.addDays( index.column()-2 );
                    result = assignment( a, d, role );
                } else {
                    result = "";
                }
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
    //kDebug()<<index.row()<<", "<<index.column()<<": "<<index.internalPointer()<<endl;
    return QVariant();
}

bool ResourceAppointmentsItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
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
                    //kDebug()<<section<<", "<<m_start<<m_end<<endl;
                    if ( section < m_columnCount && m_start.isValid() && m_end.isValid() ) {
                        QDate d = m_start.addDays( section - 2 );
                        if ( d <= m_end ) {
                            return d;
                        }
                    }
                    return QVariant();
                }
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

Appointment *ResourceAppointmentsItemModel::appointment( const QModelIndex &index ) const
{
    if ( ! m_appointments.contains( index.internalPointer() ) ) {
        return 0;
    }
    return static_cast<Appointment*>( index.internalPointer() );
}

QModelIndex ResourceAppointmentsItemModel::createAppointmentIndex( int row, int col, void *ptr ) const
{
    if ( ! m_appointments.contains( ptr ) ) {
        const_cast<ResourceAppointmentsItemModel*>( this )->m_appointments.append( ptr );
    }
    return createIndex( row, col, ptr );
}

Resource *ResourceAppointmentsItemModel::resource( const QModelIndex &index ) const
{
    if ( ! m_resources.contains( index.internalPointer() ) ) {
        return 0;
    }
    return static_cast<Resource*>( index.internalPointer() );
}

QModelIndex ResourceAppointmentsItemModel::createResourceIndex( int row, int col, void *ptr ) const
{
    if ( ! m_resources.contains( ptr ) ) {
        const_cast<ResourceAppointmentsItemModel*>( this )->m_resources.append( ptr );
    }
    return createIndex( row, col, ptr );
}

ResourceGroup *ResourceAppointmentsItemModel::resourcegroup( const QModelIndex &index ) const
{
    if ( ! m_groups.contains( index.internalPointer() ) ) {
        return 0;
    }
    return static_cast<ResourceGroup*>( index.internalPointer() );
}

QModelIndex ResourceAppointmentsItemModel::createGroupIndex( int row, int col, void *ptr ) const
{
    if ( ! m_groups.contains( ptr ) ) {
        const_cast<ResourceAppointmentsItemModel*>( this )->m_groups.append( ptr );
    }
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
        foreach( KABC::Addressee a, lst ) {
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


} // namespace KPlato

#include "kptresourceappointmentsmodel.moc"
