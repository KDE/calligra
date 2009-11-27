/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptrelationmodel.h"

#include "kptglobal.h"
#include "kptcommonstrings.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptproject.h"
#include "kptnode.h"
#include "kptrelation.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QWidget>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

#include <kdganttglobal.h>

namespace KPlato
{

QVariant RelationModel::parentName( const Relation *r, int role ) const
{
    //kDebug()<<r<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
        case Qt::EditRole: 
            return r->parent()->name();
        case Qt::TextAlignmentRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant RelationModel::childName( const Relation *r, int role ) const
{
    //kDebug()<<r<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
        case Qt::EditRole: 
            return r->child()->name();
        case Qt::TextAlignmentRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant RelationModel::type( const Relation *r, int role ) const
{
    //kDebug()<<r<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return r->typeToString( true );
        case Role::EnumList: 
            return r->typeList( true );
        case Qt::EditRole: 
        case Role::EnumListValue: 
            return (int)r->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant RelationModel::lag( const Relation *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration::Unit unit = Duration::Unit_h;
            return KGlobal::locale()->formatNumber( r->lag().toDouble( unit ), 1 ) +  Duration::unitToString( unit, true );
        }
        case Qt::EditRole:
            return r->lag().toDouble( Duration::Unit_h );
        case Role::DurationUnit:
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant RelationModel::data( const Relation *r, int property, int role ) const
{
    QVariant result;
    switch ( property ) {
        case 0: result = parentName( r, role ); break;
        case 1: result = childName( r, role ); break;
        case 2: result = type( r, role ); break;
        case 3: result = lag( r, role ); break;
        default:
            //kDebug()<<"Invalid property number: "<<property<<endl;;
            return result;
    }
    return result;
}

int RelationModel::propertyCount()
{
    return 4;
}

QVariant RelationModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case 0: return i18n( "Parent" );
            case 1: return i18n( "Child" );
            case 2: return i18n( "Type" );
            case 3: return i18n( "Lag" );
            default: return QVariant();
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::relationParent();
            case 1: return ToolTip::relationChild();
            case 2: return ToolTip::relationType();
            case 3: return ToolTip::relationLag();
            default: return QVariant();
        }
    }
    return QVariant();
}

//----------------------------
RelationItemModel::RelationItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_node( 0 ),
    m_removedRelation( 0 )
{
}

RelationItemModel::~RelationItemModel()
{
}

void RelationItemModel::slotRelationToBeAdded( Relation *relation, int, int )
{
    kDebug();
    if ( m_node == 0 || m_node != relation->child() ) {
        return;
    }
    // relations always appended
    int row = rowCount();
    beginInsertRows( QModelIndex(), row, row );
}

void RelationItemModel::slotRelationAdded( Relation *relation )
{
    kDebug();
    if ( m_node == 0 || m_node != relation->child() ) {
        return;
    }
    endInsertRows();
}

void RelationItemModel::slotRelationToBeRemoved( Relation *relation )
{
    if ( m_node == 0 || ! m_node->dependParentNodes().contains( relation ) ) {
        return;
    }
    m_removedRelation = relation;
    int row = m_node->dependParentNodes().indexOf( relation );
    kDebug()<<row;
    beginRemoveRows( QModelIndex(), row, row );
}

void RelationItemModel::slotRelationRemoved( Relation *relation )
{
    kDebug();
    if ( m_removedRelation != relation ) {
        return;
    }
    m_removedRelation = 0;
    endRemoveRows();
}

void RelationItemModel::slotRelationModified( Relation *relation )
{
    kDebug();
    if ( m_node == 0 || ! m_node->dependParentNodes().contains( relation ) ) {
        return;
    }
    int row = m_node->dependParentNodes().indexOf( relation );
    emit dataChanged( createIndex( row, 0 ), createIndex( row, columnCount()-1 ) );
}

void RelationItemModel::slotNodeToBeRemoved( Node *node )
{
    if ( node != m_node ) {
        return;
    }
    setNode( 0 );
}

void RelationItemModel::slotNodeRemoved( Node *node )
{
}

void RelationItemModel::slotLayoutChanged()
{
    //kDebug()<<node->name()<<endl;
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void RelationItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        
        disconnect( m_project, SIGNAL( relationToBeAdded( Relation*, int, int ) ), this, SLOT( slotRelationToBeAdded( Relation*, int, int ) ) );
        disconnect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
    
        disconnect( m_project, SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( slotRelationToBeRemoved( Relation* ) ) );
        disconnect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
        
        disconnect( m_project, SIGNAL( relationModified( Relation* ) ), this, SLOT( slotRelationModified( Relation* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        
        connect( m_project, SIGNAL( relationToBeAdded( Relation*, int, int ) ), this, SLOT( slotRelationToBeAdded( Relation*, int, int ) ) );
        connect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
    
        connect( m_project, SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( slotRelationToBeRemoved( Relation* ) ) );
        connect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
        
        connect( m_project, SIGNAL( relationModified( Relation* ) ), this, SLOT( slotRelationModified( Relation* ) ) );
    }
    reset();
}

void RelationItemModel::setNode( Node *node )
{
    m_node = node;
    reset();
}

Qt::ItemFlags RelationItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() ) {
        if ( m_readWrite ) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    }
    if ( m_readWrite ) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case 2: // type
                flags |= Qt::ItemIsEditable;
                break;
            case 3: // lag
                flags |= Qt::ItemIsEditable;
                break;
            default: 
                flags &= ~Qt::ItemIsEditable;
                break;
        }
    }
    return flags;
}

    
QModelIndex RelationItemModel::parent( const QModelIndex &/*index*/ ) const
{
    return QModelIndex(); // flat model
}

QModelIndex RelationItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0  ) {
        return QModelIndex();
    }
    if ( parent.isValid() ) {
        return QModelIndex(); // flat model
    }
    return createIndex( row, column );
}

bool RelationItemModel::setType( Relation *r, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Relation::Type v = Relation::Type( value.toInt() );
            //kDebug()<<v<<r->type();
            if ( v == r->type() ) {
                return false;
            }
            emit executeCommand( new ModifyRelationTypeCmd( r, v, "Modify relation type" ) );
            return true;
    }
    return false;
}

bool RelationItemModel::setLag( Relation *r, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
            Duration d( value.toList()[0].toDouble(), unit );
            kDebug()<<value.toList()[0].toDouble()<<","<<unit<<" ->"<<d.toString();
            if ( d == r->lag() ) {
                return false;
            }
            emit executeCommand( new ModifyRelationLagCmd( r, d, i18n( "Modify relation time lag" ) ) );
            return true;
        }
        default:
            break;
    }
    return false;
}

QVariant RelationItemModel::data( const QModelIndex &index, int role ) const
{
    if ( role == Qt::TextAlignmentRole ) {
        return headerData( index.column(), Qt::Horizontal, role );
    }

    QVariant result;
    Relation *r = relation( index );
    if ( r != 0 ) {
        result = m_relationmodel.data( r, index.column(), role );
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return result;
}

bool RelationItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags(index) & Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Relation *r = relation( index );
    switch (index.column()) {
        case 0: return false;
        case 1: return false;
        case 2: return setType( r, value, role );
        case 3: return setLag( r, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant RelationItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_relationmodel.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 2: return Qt::AlignCenter;
                case 3: return Qt::AlignRight;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return RelationModel::headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *RelationItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case 2: return new EnumDelegate( parent );
        case 3: return new DurationSpinBoxDelegate( parent );
        default: return 0;
    }
    return 0;
}

int RelationItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_relationmodel.propertyCount();
}

int RelationItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_node == 0 || parent.isValid() ) {
        return 0;
    }
    return m_node->numDependParentNodes();
}

Relation *RelationItemModel::relation( const QModelIndex &index ) const
{
    if ( ! index.isValid() || m_node == 0 ) {
        return 0;
    }
    return m_node->dependParentNodes().value( index.row() );
}

void RelationItemModel::slotNodeChanged( Node *node )
{
    reset();
}


} //namespace KPlato

#include "kptrelationmodel.moc"
