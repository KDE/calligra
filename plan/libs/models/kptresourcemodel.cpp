/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2011, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptresourcemodel.h"

#include "kptcommonstrings.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptdebug.h"

#include <KoIcon.h>

#include <QMimeData>
#include <QObject>
#include <QStringList>

#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kio/job.h>

#ifdef PLAN_KDEPIMLIBS_FOUND
#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>
#endif


namespace KPlato
{

//--------------------------------------

ResourceModel::ResourceModel( QObject *parent )
    : QObject( parent ),
    m_project( 0 )
{
}

ResourceModel::~ResourceModel()
{
}

const QMetaEnum ResourceModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

void ResourceModel::setProject( Project *project )
{
    m_project = project;
}

int ResourceModel::propertyCount() const
{
    return columnMap().keyCount();
}

QVariant ResourceModel::name( const Resource *res, int role ) const
{
    //kDebug(planDbg())<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->name();
        case Qt::ToolTipRole:
            if ( res->autoAllocate() ) {
                return i18nc( "@info:tooltip", "%1:<nl/>This resource will be automatically allocated to new tasks", res->name() );
            }
            return res->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::DecorationRole:
            if ( res->isBaselined() ) {
                return koIcon("view-time-schedule-baselined");
             }
             break;
        case Qt::CheckStateRole:
            return res->autoAllocate() ? Qt::Checked : Qt::Unchecked;
        default:
            break;
    }
    return QVariant();
}

QVariant ResourceModel::name( const  ResourceGroup *res, int role ) const
{
    //kDebug(planDbg())<<res->name()<<","<<role;
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

QVariant ResourceModel::type( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return res->typeToString( true );
        case Qt::EditRole:
            return res->typeToString( false );
        case Role::EnumList:
            return res->typeToStringList( true );
        case Role::EnumListValue:
            return (int)res->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::type( const ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return res->typeToString( true );
        case Qt::EditRole:
            return res->typeToString( false );
        case Role::EnumList:
            return res->typeToStringList( true );
        case Role::EnumListValue:
            return (int)res->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant ResourceModel::initials( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->initials();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::email( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->email();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::calendar( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            if ( res->type() == Resource::Type_Team ) {
                return " ";
            }
            QString s = i18n( "None" );
            Calendar *cal = res->calendar( true ); // don't check for default calendar
            if ( cal ) {
                s = cal->name();
            } else if ( res->type() == Resource::Type_Work ) {
                // Do we get a default calendar
                cal = res->calendar();
                if ( cal ) {
                    s = i18nc( "Default (calendar name)", "Default (%1)", cal->name() );
                }
            }
            return s;
        }
        case Qt::ToolTipRole: {
            if ( res->type() == Resource::Type_Team ) {
                return i18nc( "@info:tooltip", "A team resource does not have a calendar" );
            }
            QString s = i18nc( "@info:tooltip", "No calendar" );
            Calendar *cal = res->calendar( true ); // don't check for default calendar
            if ( cal ) {
                s = cal->name();
            } else if ( res->type() == Resource::Type_Work ) {
                // Do we get a default calendar
                cal = res->calendar();
                if ( cal ) {
                    s = i18nc( "@info:tooltip 1=calendar name", "Using default calendar: %1", cal->name() );
                }
            }
            return s;
        }
        case Role::EnumList: {
            Calendar *cal = m_project->defaultCalendar();
            QString s = i18n( "None" );
            if ( cal &&  res->type() == Resource::Type_Work ) {
                s = i18nc( "Default (calendar name)", "Default (%1)", cal->name() );
            }
            return QStringList() << s << m_project->calendarNames();
        }
        case Qt::EditRole:
        case Role::EnumListValue: {
            Calendar *cal = res->calendar( true ); // don't check for default calendar
            return cal == 0 ? 0 : m_project->calendarNames().indexOf( cal->name() ) + 1;
        }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::units( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->units();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::availableFrom( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( res->availableFrom() );
        case Qt::EditRole:
            return res->availableFrom();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole: {
            if ( res->availableFrom().isValid() ) {
                return i18nc( "infor:tooltip", "Available from: %1", KGlobal::locale()->formatDateTime( res->availableFrom(), KLocale::LongDate, KLocale::TimeZone ) );
            }
            return i18nc( "infor:tooltip", "Available from project target start time: %1", KGlobal::locale()->formatDateTime( m_project->constraintStartTime(), KLocale::LongDate, KLocale::TimeZone ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::availableUntil( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( res->availableUntil() );
        case Qt::EditRole:
            return res->availableUntil();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole: {
            if ( res->availableUntil().isValid() ) {
                return i18nc( "infor:tooltip", "Available until: %1", KGlobal::locale()->formatDateTime( res->availableUntil(), KLocale::LongDate, KLocale::TimeZone ) );
            }
            return i18nc( "infor:tooltip", "Available from project target finish time: %1", KGlobal::locale()->formatDateTime( m_project->constraintEndTime(), KLocale::LongDate, KLocale::TimeZone ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::normalRate( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return m_project->locale()->formatMoney( res->normalRate() );
        case Qt::EditRole:
            return res->normalRate();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
            return i18n( "Cost per hour, normal time: %1", m_project->locale()->formatMoney( res->normalRate() ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::overtimeRate( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return m_project->locale()->formatMoney( res->overtimeRate() );
        case Qt::EditRole:
            return res->overtimeRate();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
            return i18n( "Cost per hour, overtime: %1", m_project->locale()->formatMoney( res->overtimeRate() ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::account( const Resource *resource, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Account *a = resource->account();
            return a == 0 ? i18n( "None" ) : a->name();
        }
        case Qt::ToolTipRole: {
            Account *a = resource->account();
            return i18n( "Account: %1", (a == 0 ? i18n( "None" ) : a->name() ) );
        }
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = resource->account();
            return a == 0 ? 0 : ( m_project->accounts().costElements().indexOf( a->name() ) + 1 );
        }
        case Role::EnumList: {
            QStringList lst;
            lst << i18n("None");
            lst += m_project->accounts().costElements();
            return lst;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceModel::data( const Resource *resource, int property, int role ) const
{
    if ( role == Role::ObjectType ) {
        return OT_Resource;
    }
    QVariant result;
    if ( resource == 0 ) {
        return result;
    }
    switch ( property ) {
        case ResourceName: result = name( resource, role ); break;
        case ResourceType: result = type( resource, role ); break;
        case ResourceInitials: result = initials( resource, role ); break;
        case ResourceEmail: result = email( resource, role ); break;
        case ResourceCalendar: result = calendar( resource, role ); break;
        case ResourceLimit: result = units( resource, role ); break;
        case ResourceAvailableFrom: result = availableFrom( resource, role ); break;
        case ResourceAvailableUntil: result = availableUntil( resource, role ); break;
        case ResourceNormalRate: result = normalRate( resource, role ); break;
        case ResourceOvertimeRate: result = overtimeRate( resource, role ); break;
        case ResourceAccount: result = account( resource, role ); break;
        default:
            kDebug(planDbg())<<"data: invalid display value: property="<<property;
            break;
    }
    return result;
}

QVariant ResourceModel::data( const ResourceGroup *group, int property, int role ) const
{
    if ( role == Role::ObjectType ) {
        return OT_ResourceGroup;
    }
    QVariant result;
    if ( group == 0 ) {
        return result;
    }
    switch ( property ) {
        case ResourceModel::ResourceName: result = name( group, role ); break;
        case ResourceModel::ResourceType: result = type( group, role ); break;
        default:
            if ( role == Qt::DisplayRole ) {
                if ( property < propertyCount() ) {
                    result = "";
                } else {
                    kDebug(planDbg())<<"data: invalid display value column"<<property;;
                    return QVariant();
                }
            }
            break;
    }
    return result;
}


QVariant ResourceModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case ResourceName: return i18n( "Name" );
            case ResourceType: return i18n( "Type" );
            case ResourceInitials: return i18n( "Initials" );
            case ResourceEmail: return i18n( "Email" );
            case ResourceCalendar: return i18n( "Calendar" );
            case ResourceLimit: return i18n( "Limit (%)" );
            case ResourceAvailableFrom: return i18n( "Available From" );
            case ResourceAvailableUntil: return i18n( "Available Until" );
            case ResourceNormalRate: return i18n( "Normal Rate" );
            case ResourceOvertimeRate: return i18n( "Overtime Rate" );
            case ResourceAccount: return i18n( "Account" );
            default: return QVariant();
        }
    } else if ( role == Qt::TextAlignmentRole ) {
        switch (section) {
            case ResourceName:
            case ResourceType:
            case ResourceInitials:
            case ResourceEmail:
            case ResourceCalendar:
                return QVariant();
            case ResourceLimit:
                return (int)(Qt::AlignRight|Qt::AlignVCenter);
            case ResourceAvailableFrom:
            case ResourceAvailableUntil:
                return QVariant();
            case ResourceNormalRate:
            case ResourceOvertimeRate:
                return (int)(Qt::AlignRight|Qt::AlignVCenter);
            case ResourceAccount:
                return QVariant();
            default:
                return QVariant();
        }
    } else if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case ResourceName: return ToolTip::resourceName();
            case ResourceType: return ToolTip::resourceType();
            case ResourceInitials: return ToolTip::resourceInitials();
            case ResourceEmail: return ToolTip::resourceEMail();
            case ResourceCalendar: return ToolTip::resourceCalendar();
            case ResourceLimit: return ToolTip::resourceUnits();
            case ResourceAvailableFrom: return ToolTip::resourceAvailableFrom();
            case ResourceAvailableUntil: return ToolTip::resourceAvailableUntil();
            case ResourceNormalRate: return ToolTip::resourceNormalRate();
            case ResourceOvertimeRate: return ToolTip::resourceOvertimeRate();
            case ResourceAccount: return ToolTip::resourceAccount();
            default: return QVariant();
        }
    }
    return QVariant();
}

//--------------------------------------

ResourceItemModel::ResourceItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_group( 0 ),
    m_resource( 0 )
{
}

ResourceItemModel::~ResourceItemModel()
{
}

void ResourceItemModel::slotResourceToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug(planDbg())<<group->name()<<","<<row;
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    beginInsertRows( index( group ), row, row );
}

void ResourceItemModel::slotResourceInserted( const Resource *resource )
{
    //kDebug(planDbg())<<resource->name();
    Q_ASSERT( resource->parentGroup() == m_group );
#ifdef NDEBUG
    Q_UNUSED(resource)
#endif
    endInsertRows();
    m_group = 0;
    emit layoutChanged(); //HACK to make the right view react! Bug in qt?
}

void ResourceItemModel::slotResourceToBeRemoved( const Resource *resource )
{
    //kDebug(planDbg())<<resource->name();
    Q_ASSERT( m_resource == 0 );
#ifdef NDEBUG
    Q_UNUSED(resource)
#endif
    m_resource = const_cast<Resource*>(resource);
    int row = index( resource ).row();
    beginRemoveRows( index( resource->parentGroup() ), row, row );
}

void ResourceItemModel::slotResourceRemoved( const Resource *resource )
{
    //kDebug(planDbg())<<resource->name();
    Q_ASSERT( resource == m_resource );
#ifdef NDEBUG
    Q_UNUSED(resource)
#endif
    endRemoveRows();
    m_resource = 0;
}

void ResourceItemModel::slotResourceGroupToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug(planDbg())<<group->name();
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    beginInsertRows( QModelIndex(), row, row );
}

void ResourceItemModel::slotResourceGroupInserted( const ResourceGroup *group )
{
    //kDebug(planDbg())<<group->name();
    Q_ASSERT( group == m_group );
#ifdef NDEBUG
    Q_UNUSED(group)
#endif
    endInsertRows();
    m_group = 0;
}

void ResourceItemModel::slotResourceGroupToBeRemoved( const ResourceGroup *group )
{
    //kDebug(planDbg())<<group->name();
    Q_ASSERT( m_group == 0 );
    m_group = const_cast<ResourceGroup*>(group);
    int row = index( group ).row();
    beginRemoveRows( QModelIndex(), row, row );
}

void ResourceItemModel::slotResourceGroupRemoved( const ResourceGroup *group )
{
    //kDebug(planDbg())<<group->name();
    Q_ASSERT( group == m_group );
#ifdef NDEBUG
    Q_UNUSED(group)
#endif

    endRemoveRows();
    m_group = 0;
}

void ResourceItemModel::slotLayoutChanged()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void ResourceItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
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
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
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
    }
    m_model.setProject( m_project );
    reset();
}

Qt::ItemFlags ResourceItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        //kDebug(planDbg())<<"read only"<<flags;
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( !index.isValid() ) {
        //kDebug(planDbg())<<"invalid"<<flags;
        return flags;
    }
    Resource *r = qobject_cast<Resource*>( object ( index ) );
    if ( r != 0 ) {
        flags |= Qt::ItemIsDragEnabled;
        switch ( index.column() ) {
            case ResourceModel::ResourceName:
                flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
                break;
            case ResourceModel::ResourceType:
                if ( ! r->isBaselined() ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case ResourceModel::ResourceAccount:
                if ( ! r->isBaselined() ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case ResourceModel::ResourceNormalRate:
                if ( ! r->isBaselined() ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case ResourceModel::ResourceOvertimeRate:
                if ( ! r->isBaselined() ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            default:
                flags |= Qt::ItemIsEditable;
        }
        //kDebug(planDbg())<<"resource"<<flags;
    } else if ( qobject_cast<ResourceGroup*>( object( index ) ) ) {
        flags |= Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case ResourceModel::ResourceName: flags |= Qt::ItemIsEditable; break;
            case ResourceModel::ResourceType: flags |= Qt::ItemIsEditable; break;
            default: flags &= ~Qt::ItemIsEditable;
        }
        //kDebug(planDbg())<<"group"<<flags;
    }
    return flags;
}


QModelIndex ResourceItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug(planDbg())<<index.internalPointer()<<":"<<index.row()<<","<<index.column();

    Resource *r = qobject_cast<Resource*>( object( index ) );
    if ( r && r->parentGroup() ) {
        // only resources have parent
        int row = m_project->indexOf(  r->parentGroup() );
        return createIndex( row, 0, r->parentGroup() );
    }

    return QModelIndex();
}

QModelIndex ResourceItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row < m_project->numResourceGroups() ) {
            return createIndex( row, column, m_project->resourceGroupAt( row ) );
        }
        return QModelIndex();
    }
    QObject *p = object( parent );
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        if ( row < g->numResources() ) {
            return createIndex( row, column, g->resourceAt( row ) );
        }
        return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex ResourceItemModel::index( const Resource *resource, int column ) const
{
    if ( m_project == 0 || resource == 0 ) {
        return QModelIndex();
    }
    Resource *r = const_cast<Resource*>(resource);
    int row = -1;
    ResourceGroup *par = r->parentGroup();
    if ( par ) {
        row = par->indexOf( r );
        return createIndex( row, column, r );
    }
    return QModelIndex();
}

QModelIndex ResourceItemModel::index( const ResourceGroup *group, int column ) const
{
    if ( m_project == 0 || group == 0 ) {
        return QModelIndex();
    }
    ResourceGroup *g = const_cast<ResourceGroup*>(group);
    int row = m_project->indexOf( g );
    return createIndex( row, column, g );

}

int ResourceItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_model.propertyCount();
}

int ResourceItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    if ( ! parent.isValid() ) {
        return m_project->numResourceGroups();
    }
    QObject *p = object( parent );
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        return g->numResources();
    }
    return 0;
}

QVariant ResourceItemModel::name( const  ResourceGroup *res, int role ) const
{
    //kDebug(planDbg())<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setName( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == res->name() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceNameCmd( res, value.toString(), i18nc( "(qtundo-format)", "Modify resource name" ) ) );
            return true;
        case Qt::CheckStateRole:
            emit executeCommand( new ModifyResourceAutoAllocateCmd( res, value.toBool(), i18nc( "(qtundo-format)", "Modify resource auto allocate" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setName( ResourceGroup *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == res->name() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceGroupNameCmd( res, value.toString(), i18nc( "(qtundo-format)", "Modify resourcegroup name" ) ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::type( const ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return res->typeToString( true );
        case Qt::EditRole:
            return res->typeToString( false );
        case Role::EnumList:
            return res->typeToStringList( true );
        case Role::EnumListValue:
            return (int)res->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setType( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Resource::Type v;
            QStringList lst = res->typeToStringList( false );
            if ( lst.contains( value.toString() ) ) {
                v = static_cast<Resource::Type>( lst.indexOf( value.toString() ) );
            } else {
                v = static_cast<Resource::Type>( value.toInt() );
            }
            if ( v == res->type() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceTypeCmd( res, v, i18nc( "(qtundo-format)", "Modify resource type" ) ) );
            return true;
        }
    }
    return false;
}

bool ResourceItemModel::setType( ResourceGroup *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            ResourceGroup::Type v;
            QStringList lst = res->typeToStringList( false );
            if ( lst.contains( value.toString() ) ) {
                v = static_cast<ResourceGroup::Type>( lst.indexOf( value.toString() ) );
            } else {
                v = static_cast<ResourceGroup::Type>( value.toInt() );
            }
            if ( v == res->type() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceGroupTypeCmd( res, v, i18nc( "(qtundo-format)", "Modify resourcegroup type" ) ) );
            return true;
        }
    }
    return false;
}

bool ResourceItemModel::setInitials( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == res->initials() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceInitialsCmd( res, value.toString(), i18nc( "(qtundo-format)", "Modify resource initials" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setEmail( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == res->email() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceEmailCmd( res, value.toString(), i18nc( "(qtundo-format)", "Modify resource email" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setCalendar( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
        {
            Calendar *c = 0;
            if ( value.toInt() > 0 ) {
                QStringList lst = m_model.calendar( res, Role::EnumList ).toStringList();
                if ( value.toInt() < lst.count() ) {
                    c = m_project->calendarByName( lst.at( value.toInt() ) );
                }
            }
            if ( c == res->calendar( true ) ) {
                return false;
            }
            emit executeCommand( new ModifyResourceCalendarCmd( res, c, i18nc( "(qtundo-format)", "Modify resource calendar" ) ) );
            return true;
        }
    }
    return false;
}


bool ResourceItemModel::setUnits( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == res->units() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceUnitsCmd( res, value.toInt(), i18nc( "(qtundo-format)", "Modify resource available units" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setAvailableFrom( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == res->availableFrom() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceAvailableFromCmd( res, value.toDateTime(), i18nc( "(qtundo-format)", "Modify resource available from" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setAvailableUntil( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == res->availableUntil() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceAvailableUntilCmd( res, value.toDateTime(), i18nc( "(qtundo-format)", "Modify resource available until" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setNormalRate( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDouble() == res->normalRate() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceNormalRateCmd( res, value.toDouble(), i18nc( "(qtundo-format)", "Modify resource normal rate" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setOvertimeRate( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDouble() == res->overtimeRate() ) {
                return false;
            }
            emit executeCommand( new ModifyResourceOvertimeRateCmd( res, value.toDouble(), i18nc( "(qtundo-format)", "Modify resource overtime rate" ) ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setAccount( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Account *a = 0;
            if ( value.type() == QVariant::Int ) {
                QStringList lst = m_model.account( res, Role::EnumList ).toStringList();
                if ( value.toInt() >= lst.count() ) {
                    return false;
                }
                a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            } else if ( value.type() == QVariant::String ) {
                a = m_project->accounts().findAccount( value.toString() );
            }
            Account *old = res->account();
            if ( old != a ) {
                emit executeCommand( new ResourceModifyAccountCmd( *res, old, a, i18nc( "(qtundo-format)", "Modify resource account" ) ) );
                return true;
            }
        }
        default:
            break;
    }
    return false;
}

QVariant ResourceItemModel::notUsed( const ResourceGroup *, int role ) const
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

QVariant ResourceItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    QObject *obj = object( index );
    if ( obj == 0 ) {
        return QVariant();
    }
    if ( role == Qt::TextAlignmentRole ) {
        // use same alignment as in header (headers always horizontal)
        return headerData( index.column(), Qt::Horizontal, role );
    }
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        result = m_model.data( r, index.column(), role );
    } else {
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            result = m_model.data( g, index.column(), role );
        }
    }
    return result;
}

bool ResourceItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags( index ) &Qt::ItemIsEditable ) == 0 ||  ! ( role == Qt::EditRole || role == Qt::CheckStateRole ) ) {
        return false;
    }
    QObject *obj = object( index );
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        switch (index.column()) {
            case ResourceModel::ResourceName: return setName( r, value, role );
            case ResourceModel::ResourceType: return setType( r, value, role );
            case ResourceModel::ResourceInitials: return setInitials( r, value, role );
            case ResourceModel::ResourceEmail: return setEmail( r, value, role );
            case ResourceModel::ResourceCalendar: return setCalendar( r, value, role );
            case ResourceModel::ResourceLimit: return setUnits( r, value, role );
            case ResourceModel::ResourceAvailableFrom: return setAvailableFrom( r, value, role );
            case ResourceModel::ResourceAvailableUntil: return setAvailableUntil( r, value, role );
            case ResourceModel::ResourceNormalRate: return setNormalRate( r, value, role );
            case ResourceModel::ResourceOvertimeRate: return setOvertimeRate( r, value, role );
            case ResourceModel::ResourceAccount: return setAccount( r, value, role );
            default:
                qWarning("data: invalid display value column %d", index.column());
                return false;
        }
    } else {
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            switch (index.column()) {
                case ResourceModel::ResourceName: return setName( g, value, role );
                case ResourceModel::ResourceType: return setType( g, value, role );
                default:
                    qWarning("data: invalid display value column %d", index.column());
                    return false;
            }
        }
    }
    return false;
}

QVariant ResourceItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole || role == Qt::TextAlignmentRole ) {
            return m_model.headerData( section, role );
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return m_model.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *ResourceItemModel::createDelegate( int col, QWidget *parent ) const
{
    switch ( col ) {
        case ResourceModel::ResourceType: return new EnumDelegate( parent );
        case ResourceModel::ResourceCalendar: return new EnumDelegate( parent );
        case ResourceModel::ResourceAvailableFrom: return new DateTimeCalendarDelegate( parent );
        case ResourceModel::ResourceAvailableUntil: return new DateTimeCalendarDelegate( parent );
        case ResourceModel::ResourceAccount: return new EnumDelegate( parent );
        default: break;
    }
    return 0;
}

QObject *ResourceItemModel::object( const QModelIndex &index ) const
{
    QObject *o = 0;
    if ( index.isValid() ) {
        o = static_cast<QObject*>( index.internalPointer() );
        Q_ASSERT( o );
    }
    return o;
}

ResourceGroup *ResourceItemModel::group( const QModelIndex &index ) const
{
    return qobject_cast<ResourceGroup*>( object( index ) );
}

Resource *ResourceItemModel::resource( const QModelIndex &index ) const
{
    return qobject_cast<Resource*>( object( index ) );
}

void ResourceItemModel::slotCalendarChanged( Calendar* )
{
    foreach ( Resource *r, m_project->resourceList() ) {
        if ( r->calendar( true ) == 0 ) {
            slotResourceChanged( r );
        }
    }
}

void ResourceItemModel::slotResourceChanged( Resource *res )
{
    ResourceGroup *g = res->parentGroup();
    if ( g ) {
        int row = g->indexOf( res );
        emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
        return;
    }
}

void ResourceItemModel::slotResourceGroupChanged( ResourceGroup *res )
{
    Project *p = res->project();
    if ( p ) {
        int row = p->resourceGroups().indexOf( res );
        emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
    }
}

Qt::DropActions ResourceItemModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

bool ResourceItemModel::dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data )
{
    Q_UNUSED(data);

    //kDebug(planDbg())<<index<<data;
    // TODO: if internal, don't allow dropping on my own parent
    switch ( dropIndicatorPosition ) {
        case ItemModelBase::OnItem:
            return qobject_cast<ResourceGroup*>( object( index ) ); // Allow only on group
        default:
            break;
    }
    return false;
}

QStringList ResourceItemModel::mimeTypes() const
{
    return QStringList()
#ifdef PLAN_KDEPIMLIBS_FOUND
            << "text/x-vcard"
            << "text/directory"
            << "text/uri-list"
#endif
            << "application/x-vnd.kde.plan.resourceitemmodel.internal";
}

void ResourceItemModel::slotDataArrived( KIO::Job *job, const QByteArray &data  )
{
    if ( m_dropDataMap.contains( job ) ) {
        m_dropDataMap[ job ].data += data;
    }
}

void ResourceItemModel::slotJobFinished( KJob *job )
{
    if ( job->error() || ! m_dropDataMap.contains( job ) ) {
        kDebug(planDbg())<<(job->error() ? "Job error":"Error: no such job");
    } else if ( KMimeType::findByContent( m_dropDataMap[ job ].data )->is( "text/x-vcard" ) ) {
        ResourceGroup *g = 0;
        if ( m_dropDataMap[ job ].parent.isValid() ) {
            g = qobject_cast<ResourceGroup*>( object( m_dropDataMap[ job ].parent ) );
        } else {
            g = qobject_cast<ResourceGroup*>( object( index( m_dropDataMap[ job ].row, m_dropDataMap[ job ].column, m_dropDataMap[ job ].parent ) ) );
        }
        if ( g == 0 ) {
            kDebug(planDbg())<<"No group"<<m_dropDataMap[ job ].row<<m_dropDataMap[ job ].column<<m_dropDataMap[ job ].parent;
        } else {
            createResources( g, m_dropDataMap[ job ].data );
        }
    }
    if ( m_dropDataMap.contains( job ) ) {
        m_dropDataMap.remove( job );
    }
    disconnect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobFinished(KJob*)));
    disconnect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), this, SLOT(slotDataArrived(KIO::Job*, const QByteArray&)));
}

bool ResourceItemModel::createResources( ResourceGroup *group, const QByteArray &data )
{
#ifdef PLAN_KDEPIMLIBS_FOUND
    KABC::VCardConverter vc;
    KABC::Addressee::List lst = vc.parseVCards( data );
    MacroCommand *m = new MacroCommand( i18ncp( "(qtundo-format)", "Add resource from address book", "Add %1 resources from address book", lst.count() ) );
    foreach( const KABC::Addressee &a, lst ) {
        Resource *r = new Resource();
        QString uid = a.uid();
        if ( ! m_project->findResource( uid ) ) {
            r->setId( uid );
        }
        r->setName( a.formattedName() );
        r->setEmail( a.preferredEmail() );
        m->addCommand( new AddResourceCmd( group, r ) );
    }
    if ( m->isEmpty() ) {
        delete m;
        return false;
    }
    emit executeCommand( m );
    return true;
#else
    return false;
#endif
}

bool ResourceItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent )
{
    kDebug(planDbg())<<row<<column<<parent;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if (column > 0) {
        return false;
    }
    ResourceGroup *g = 0;
    if ( parent.isValid() ) {
        g = qobject_cast<ResourceGroup*>( object( parent ) );
    } else {
        g = qobject_cast<ResourceGroup*>( object( index( row, column, parent ) ) );
    }
    if ( g == 0 ) {
        kDebug(planDbg())<<"No group"<<row<<column<<parent;
        return false;
    }
    kDebug(planDbg())<<data->formats()<<g->name();
    if ( data->hasFormat( "application/x-vnd.kde.plan.resourceitemmodel.internal" ) ) {
        kDebug(planDbg())<<action<<Qt::MoveAction;
        if ( action == Qt::MoveAction ) {
            MacroCommand *m = 0;
            QByteArray encodedData = data->data( "application/x-vnd.kde.plan.resourceitemmodel.internal" );
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            int i = 0;
            foreach ( Resource *r, resourceList( stream ) ) {
                if ( r->parentGroup() == g ) {
                    continue;
                }
                if ( m == 0 ) m = new MacroCommand( "" );
                m->addCommand( new MoveResourceCmd( g, r ) );
                ++i;
            }
            if ( m ) {
                QString msg = i18np( "Move resource", "Move %1 resources", i );
                MacroCommand *c = new MacroCommand( msg );
                c->addCommand( m );
                emit executeCommand( c );
            }
            return true;
        }
        if ( action == Qt::CopyAction ) {
            MacroCommand *m = 0;
            QByteArray encodedData = data->data( "application/x-vnd.kde.plan.resourceitemmodel.internal" );
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            int i = 0;
            foreach ( Resource *r, resourceList( stream ) ) {
                Resource *nr = new Resource( r );
                if ( m == 0 ) m = new MacroCommand( "" );
                m->addCommand( new AddResourceCmd( g, nr ) );
                ++i;
            }
            if ( m ) {
                QString msg = i18np( "Copy resource", "Copy %1 resources", i );
                MacroCommand *c = new MacroCommand( msg );
                c->addCommand( m );
                emit executeCommand( c );
            }
            return true;
        }
        return true;
    }
    if ( data->hasFormat( "text/x-vcard" ) || data->hasFormat( "text/directory" ) ) {
        if ( action != Qt::CopyAction ) {
            return false;
        }
        QString f = data->hasFormat( "text/x-vcard" ) ? "text/x-vcard" : "text/directory";
        return createResources( g, data->data( f ) );
    }
    if ( data->hasFormat( "text/uri-list" ) ) {
        const KUrl::List urls = KUrl::List::fromMimeData( data );
        if ( urls.isEmpty() ) {
            return false;
        }
        bool result = false;
        foreach ( const KUrl &url, urls ) {
            if ( url.protocol() != "akonadi" || ! KIO::NetAccess::exists( url, KIO::NetAccess::SourceSide, 0 ) ) {
                kDebug(planDbg())<<url<<"is not 'akonadi' or does not exist";
                continue;
            }
            KIO::TransferJob *job = KIO::get( url, KIO::NoReload, KIO::HideProgressInfo );
            bool res = connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), this, SLOT(slotDataArrived(KIO::Job*, const QByteArray&)));
            Q_ASSERT( res );
	    Q_UNUSED( res );
            res = connect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobFinished(KJob*)));
            Q_ASSERT( res );

            m_dropDataMap[ job ].action = action;
            m_dropDataMap[ job ].row = row;
            m_dropDataMap[ job ].column = column;
            m_dropDataMap[ job ].parent = parent;

            job->start();
            result = true;
        }
        return result;
    }
    return false;
}

QList<Resource*> ResourceItemModel::resourceList( QDataStream &stream )
{
    QList<Resource*> lst;
    while (!stream.atEnd()) {
        QString id;
        stream >> id;
        Resource *r = m_project->findResource( id );
        if ( r ) {
            lst << r;
        }
    }
    kDebug(planDbg())<<lst;
    return lst;
}

QMimeData *ResourceItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (const QModelIndex &index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            //kDebug(planDbg())<<index.row();
            Resource *r = ::qobject_cast<Resource*>( object( index ) );
            if ( r ) {
                rows << index.row();
                stream << r->id();
            } else if ( ::qobject_cast<ResourceGroup*>( object( index ) ) ) {
                rows.clear();
                break;
            }
        }
    }
    if ( rows.isEmpty() ) {
        delete m;
        return 0;
    }
    m->setData("application/x-vnd.kde.plan.resourceitemmodel.internal", encodedData);
    return m;
}

QModelIndex ResourceItemModel::insertGroup( ResourceGroup *g )
{
    //kDebug(planDbg());
    emit executeCommand( new AddResourceGroupCmd( m_project, g, i18nc( "(qtundo-format)", "Add resource group" ) ) );
    int row = m_project->resourceGroups().indexOf( g );
    if ( row != -1 ) {
        return createIndex( row, 0, g );
    }
    return QModelIndex();
}

QModelIndex ResourceItemModel::insertResource( ResourceGroup *g, Resource *r, Resource * /*after*/ )
{
    //kDebug(planDbg());
    emit executeCommand( new AddResourceCmd( g, r, i18nc( "(qtundo-format)", "Add resource" ) ) );
    int row = g->indexOf( r );
    if ( row != -1 ) {
        return createIndex( row, 0, r );
    }
    return QModelIndex();
}

int ResourceItemModel::sortRole( int column ) const
{
    switch ( column ) {
        case ResourceModel::ResourceAvailableFrom:
        case ResourceModel::ResourceAvailableUntil:
            return Qt::EditRole;
        default:
            break;
    }
    return Qt::DisplayRole;
}


//-------------------
ResourceItemSFModel::ResourceItemSFModel( QObject *parent )
    : QSortFilterProxyModel( parent )
{
    setDynamicSortFilter( true );
    setSourceModel( new ResourceItemModel( this ) );
}

void ResourceItemSFModel::setProject( Project *project )
{
    static_cast<ResourceItemModel*>( sourceModel() )->setProject( project );
}

Resource *ResourceItemSFModel::resource( const QModelIndex &idx ) const
{
    return static_cast<ResourceItemModel*>( sourceModel() )->resource( mapToSource( idx ) );
}

QModelIndex ResourceItemSFModel::index( Resource *r ) const
{
    return mapFromSource( static_cast<ResourceItemModel*>( sourceModel() )->index( r ) );
}

Qt::ItemFlags ResourceItemSFModel::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags f = QSortFilterProxyModel::flags( index );
    if ( index.isValid() && ! parent( index ).isValid() ) {
        // group, not selectable
        f &= ~Qt::ItemIsSelectable;
    }
    return f;
}

void ResourceItemSFModel::addFilteredResource( const Resource *r )
{
    if ( ! m_filteredResources.contains( r ) ) {
        m_filteredResources << r;
    }
}

bool ResourceItemSFModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    //TODO make this general filter
    ResourceItemModel *m = static_cast<ResourceItemModel*>( sourceModel() );
    if ( m->index( source_row, ResourceModel::ResourceType, source_parent ).data( Role::EnumListValue ).toInt() == ResourceGroup::Type_Work ) {
        return false;
    }
    QModelIndex idx = m->index( source_row, 0, source_parent );
    return ! m_filteredResources.contains( m->resource( idx ) );
}

//-----------------------
AllocatedResourceItemModel::AllocatedResourceItemModel( QObject *parent )
    : QSortFilterProxyModel( parent ),
    m_task( 0 )
{
    setDynamicSortFilter( true );
    setSourceModel( new ResourceItemModel( this ) );
}

int AllocatedResourceItemModel::columnCount( const QModelIndex &idx ) const
{
    return 2;
}

Project *AllocatedResourceItemModel::project() const
{
    return static_cast<ResourceItemModel*>( sourceModel() )->project();
}

void AllocatedResourceItemModel::setProject( Project *project )
{
    kDebug(planDbg())<<this->project()<<"="<<project;
    Project *p =this->project();
    if ( p ) {
        disconnect(p, SIGNAL(nodeChanged(Node*)), this, SLOT(slotNodeChanged(Node*)));
    }
    static_cast<ResourceItemModel*>( sourceModel() )->setProject( project );
    if ( project ) {
        connect(project, SIGNAL(nodeChanged(Node*)), this, SLOT(slotNodeChanged(Node*)));
    }
    kDebug(planDbg())<<rowCount()<<":"<<sourceModel()->rowCount();
}

void AllocatedResourceItemModel::reset()
{
    QSortFilterProxyModel::reset();
    emit expandAll();
    emit resizeColumnToContents( 0 );
}

void AllocatedResourceItemModel::slotNodeChanged( Node *n )
{
    kDebug(planDbg())<<(n==m_task)<<n<<n->name();
    if ( n != m_task ) {
        return;
    }
    reset();
}

Task *AllocatedResourceItemModel::task() const
{
    return m_task;
}

void AllocatedResourceItemModel::setTask( Task *task )
{
    kDebug(planDbg())<<m_task<<"="<<task<<(task?task->name():"");
    m_task = task;
    reset();
    kDebug(planDbg())<<rowCount()<<":"<<sourceModel()->rowCount();
}

QObject* AllocatedResourceItemModel::object(const QModelIndex& idx) const
{
    return static_cast<ResourceItemModel*>( sourceModel() )->object( mapToSource( idx ) );
}

Resource *AllocatedResourceItemModel::resource( const QModelIndex &idx ) const
{
    return qobject_cast<Resource*>( object( idx ) );
}

QModelIndex AllocatedResourceItemModel::index( Resource *r ) const
{
    return mapFromSource( static_cast<ResourceItemModel*>( sourceModel() )->index( r ) );
}

Qt::ItemFlags AllocatedResourceItemModel::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags f = QSortFilterProxyModel::flags( index );
    f &= ~Qt::ItemIsUserCheckable;
    return f;
}

QVariant AllocatedResourceItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( section == 1 ) {
        if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
            return i18nc( "@title:column", "Allocation" );
        }
        return QVariant();
    }
    return QSortFilterProxyModel::headerData( section, orientation, role );
}

QVariant AllocatedResourceItemModel::allocation( const Resource *res, int role ) const
{
    ResourceRequest *rr = m_task->requests().find( res );
    ResourceGroupRequest *gr = m_task->requests().find( res->parentGroup() );
    if ( rr == 0 || gr == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole: {
        case Qt::EditRole:
            // xgettext: no-c-format
            return i18nc( "<value>%", "%1%",rr->units() );
        }
        case Qt::ToolTipRole: {
            if ( rr->units() == 0 ) {
                return i18nc( "@info:tooltip", "Not allocated" );
            }
            return i18nc( "@info:tooltip", "%1 allocated out of %2 available", gr->count(), res->parentGroup()->numResources() );
        }
        default:
            break;
    }
    return QVariant();
}

QVariant AllocatedResourceItemModel::allocation( const ResourceGroup *res, int role ) const
{
    ResourceGroupRequest *gr = m_task->requests().find( res );
    if ( gr == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return QString( "%1 (%2)" ).arg( gr->units() ).arg( gr->count() );
        case Qt::ToolTipRole: {
            QString s1 = i18ncp( "@info:tooltip",
                                 "%1 resource requested for dynamic allocation",
                                 "%1 resources requested for dynamic allocation",
                                 gr->units() );
            QString s2 = i18ncp( "@info:tooltip",
                                 "%1 resource allocated",
                                 "%1 resources allocated",
                                 gr->count() );

            return i18nc( "@info:tooltip", "%1<nl/>%2", s1, s2 );
        }
        case Qt::WhatsThisRole: {
            return i18nc( "@info:whatsthis",
                          "<title>Group allocations</title>"
                          "<para>You can allocate a number of resources from a group and let"
                          " the scheduler select from the available resources at the time of scheduling.</para>"
                          " These dynamically allocated resources will be in addition to any resource you have allocated specifically." );
        }
        case Role::Minimum: {
            return 0;
        }
        case Role::Maximum: {
            return res->numResources() - gr->units();
        }
        default:
            break;
    }
    return QVariant();
}

QVariant AllocatedResourceItemModel::data(const QModelIndex& idx, int role) const
{
    if ( m_task == 0 || role == Qt::CheckStateRole || role == Qt::DecorationRole ) {
        return QVariant();
    }
    if ( idx.column() == 1 ) {
        switch ( role ) {
            case Qt::TextAlignmentRole:
                return Qt::AlignLeft;
            default: {
                QObject *o = object( idx );
                Resource *r = qobject_cast<Resource*>( o );
                if ( r ) {
                    return allocation( r, role );
                }
                ResourceGroup *g = qobject_cast<ResourceGroup*>( o );
                if ( g ) {
                    return allocation( g, role );
                }
                break;
            }
            return QVariant();
        }
    }
    return QSortFilterProxyModel::data( idx, role );
}

bool AllocatedResourceItemModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    if ( m_task == 0 ) {
        return false;
    }
    QModelIndex idx = sourceModel()->index( source_row, 0, source_parent );
    if ( ! idx.isValid() ) {
        return false;
    }
    bool result = false;
    const ResourceRequestCollection &req = m_task->requests();
    if ( source_parent.isValid() ) {
        const Resource *r = static_cast<ResourceItemModel*>( sourceModel() )->resource( idx );
        result = (bool) req.find( r );
    } else {
        const ResourceGroup *g = static_cast<ResourceItemModel*>( sourceModel() )->group( idx );
        ResourceGroupRequest *gr = req.find( g );
        result = (bool) gr && ( gr->units() > 0 || gr->count() > 0 );
    }
    kDebug(planDbg())<<result<<":"<<source_parent<<idx;
    return result;
}

} // namespace KPlato

#include "kptresourcemodel.moc"
