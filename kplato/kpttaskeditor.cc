/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#include "kpttaskeditor.h"

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kptcommand.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptview.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QHeaderView>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMap>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QVBoxLayout>
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

namespace KPlato
{

NodeItemModel::NodeItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_node( 0 )
{
}

NodeItemModel::~NodeItemModel()
{
}
    
void NodeItemModel::slotNodeToBeInserted( Node *parent, int row )
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    Q_ASSERT( m_node == 0 );
    m_node = parent;
    beginInsertRows( index( parent ), row, row );
}

void NodeItemModel::slotNodeInserted( Node *node )
{
    //kDebug()<<k_funcinfo<<node->parentNode->name()<<"-->"<<node->name()<<endl;
    Q_ASSERT( node->parentNode() == m_node );
    endInsertRows();
    m_node = 0;
}

void NodeItemModel::slotNodeToBeRemoved( Node *node )
{
    kDebug()<<k_funcinfo<<node->name()<<endl;
    Q_ASSERT( m_node == 0 );
    m_node = node;
    int row = index( node ).row();
    beginRemoveRows( index( node->parentNode() ), row, row );
}

void NodeItemModel::slotNodeRemoved( Node *node )
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    Q_ASSERT( node == m_node );
    endRemoveRows();
    m_node = 0;
}

void NodeItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node* ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
}
    
Qt::ItemFlags NodeItemModel::flags( const QModelIndex &index ) const
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
            case 1: break; // Node type
            
            case 3: // allocation
            case 4: // estimateType
            case 5: // estimate
            case 6: // optimisticRatio
            case 7: // pessimisticRatio
            {
                Node *n = node( index );
                if ( n && (n->type() == Node::Type_Task || n->type() == Node::Type_Milestone )) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case 8: // risktype
            {
                Node *n = node( index );
                if ( n && n->type() == Node::Type_Task ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case 10: { // constraint start
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case 11: { // constraint end
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustFinishOn || c == Node::FinishNotLater || c ==  Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case 12: // running account
            case 13: // startup account
            case 14: // startup cost
            case 15: // shutdown account
            case 16: { // shutdown cost
                Node *n = node( index );
                if ( n && (n->type() == Node::Type_Task || n->type() == Node::Type_Milestone) ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            default: flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}

    
QModelIndex NodeItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
    Node *p = node( index )->parentNode();
    if ( p == 0 || p->type() == Node::Type_Project ) {
        return QModelIndex();
    }
    int row = p->parentNode()->findChildNode( p );
    return createIndex( row, 0, p );
}

bool NodeItemModel::hasChildren( const QModelIndex &parent ) const
{
    Node *p = node( parent );
    return p->numChildren() > 0;
}

QModelIndex NodeItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    Node *p = node( parent );
    if ( row >= p->numChildren() ) {
        return QModelIndex();
    }
    // now get the internal pointer for the index
    Node *n = p->childNode( row );

    return createIndex(row, column, n);
}

QModelIndex NodeItemModel::index( const Node *node ) const
{
    if ( m_project == 0 || node == 0 ) {
        return QModelIndex();
    }
    Node *par = node->parentNode();
    if ( par ) {
        kDebug()<<k_funcinfo<<par<<"-->"<<node<<endl;
        return createIndex( par->indexOf( node ), 0, const_cast<Node*>(node) );
    }
    kDebug()<<k_funcinfo<<node<<endl;
    return QModelIndex();
}

bool NodeItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
    //TODO
    kDebug()<<k_funcinfo<<endl;
    return true;
}

bool NodeItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
    //TODO
    kDebug()<<k_funcinfo<<endl;
    return true;
}
    
QVariant NodeItemModel::name( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setName( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new NodeModifyNameCmd( m_part, *node, value.toString(), "Modify task name" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::leader( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->leader();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setLeader( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new NodeModifyLeaderCmd( m_part, *node, value.toString(), "Modify task responsible" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::allocation( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->requestNameList().join(",");
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setAllocation( Node *node, const QVariant &value, int role )
{
    Task *task = qobject_cast<Task*>( node );
    if ( task == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
        {
            KMacroCommand *cmd = 0;
            QStringList res = m_project->resourceNameList();
            QStringList req = node->requestNameList();
            QStringList alloc;
            foreach ( QString s, value.toString().split( QRegExp(" *, *"), QString::SkipEmptyParts ) ) {
                alloc << s.trimmed();
            }
            // first add all new resources (to "default" group)
            ResourceGroup *pargr = m_project->groupByName( i18n( "Resources" ) );
            foreach ( QString s, alloc ) {
                Resource *r = m_project->resourceByName( s.trimmed() );
                if ( r != 0 ) {
                    continue;
                }
                if ( cmd == 0 ) cmd = new KMacroCommand( i18n( "Add Resource" ) );
                if ( pargr == 0 ) {
                    pargr = new ResourceGroup();
                    pargr->setName( i18n( "Resources" ) );
                    cmd->addCommand( new AddResourceGroupCmd( m_part, m_project, pargr ) );
                    //kDebug()<<k_funcinfo<<"add group: "<<pargr->name()<<endl;
                }
                r = new Resource();
                r->setName( s.trimmed() );
                cmd->addCommand( new AddResourceCmd( m_part, pargr, r ) );
                //kDebug()<<k_funcinfo<<"add resource: "<<r->name()<<endl;
                m_part->addCommand( cmd );
                cmd = 0;
            }
            
            QString c = i18n( "Modify Resource Allocations" );
            // Handle deleted requests
            foreach ( QString s, req ) {
                // if a request is not in alloc, it must have been be removed by the user
                if ( alloc.indexOf( s ) == -1 ) {
                    // remove removed resource request
                    ResourceRequest *r = node->resourceRequest( s );
                    if ( r ) {
                        if ( cmd == 0 ) cmd = new KMacroCommand( c );
                        //kDebug()<<k_funcinfo<<"delete request: "<<r->resource()->name()<<" group: "<<r->parent()->group()->name()<<endl;
                        cmd->addCommand( new RemoveResourceRequestCmd( m_part, r->parent(), r ) );
                    }
                }
            }
            // Handle new requests
            foreach ( QString s, alloc ) {
                // if an allocation is not in req, it must be added
                if ( req.indexOf( s ) == -1 ) {
                    ResourceGroup *pargr = 0;
                    Resource *r = m_project->resourceByName( s );
                    if ( r == 0 ) {
                        // Handle request to non exixting resource
                        pargr = m_project->groupByName( i18n( "Resources" ) );
                        if ( pargr == 0 ) {
                            pargr = new ResourceGroup();
                            pargr->setName( i18n( "Resources" ) );
                            cmd->addCommand( new AddResourceGroupCmd( m_part, m_project, pargr ) );
                            //kDebug()<<k_funcinfo<<"add group: "<<pargr->name()<<endl;
                        }
                        r = new Resource();
                        r->setName( s );
                        cmd->addCommand( new AddResourceCmd( m_part, pargr, r ) );
                        //kDebug()<<k_funcinfo<<"add resource: "<<r->name()<<endl;
                        m_part->addCommand( cmd );
                        cmd = 0;
                    } else {
                        pargr = r->parentGroup();
                        //kDebug()<<k_funcinfo<<"add '"<<r->name()<<"' to group: "<<pargr<<endl;
                    }
                    // add request
                    ResourceGroupRequest *g = node->resourceGroupRequest( pargr );
                    if ( g == 0 ) {
                        // create a group request
                        if ( cmd == 0 ) cmd = new KMacroCommand( c );
                        g = new ResourceGroupRequest( pargr );
                        cmd->addCommand( new AddResourceGroupRequestCmd( m_part, *task, g ) );
                        //kDebug()<<k_funcinfo<<"add group request: "<<g<<endl;
                    }
                    if ( cmd == 0 ) cmd = new KMacroCommand( c );
                    cmd->addCommand( new AddResourceRequestCmd( m_part, g, new ResourceRequest( r, 100 ) ) );
                    kDebug()<<k_funcinfo<<"add request: "<<r->name()<<" group: "<<g<<endl;
                }
            }
            if ( cmd ) {
                m_part->addCommand( cmd );
            }
            return true;
        }
    }
    return false;
}

QVariant NodeItemModel::description( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->description();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setDescription( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new NodeModifyDescriptionCmd( m_part, *node, value.toString(), "Modify task description" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::type( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->typeToString( true );
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setType( Node *, const QVariant &, int )
{
    return false;
}

QVariant NodeItemModel::constraint( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return node->constraintToString( true );
        case Role::EnumList: 
            return Node::constraintList( true );
        case Qt::EditRole: 
        case Role::EnumListValue: 
            return (int)node->constraint();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}
bool NodeItemModel::setConstraint( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Node::ConstraintType v = Node::ConstraintType( value.toInt() );
            kDebug()<<k_funcinfo<<v<<endl;
            m_part->addCommand( new NodeModifyConstraintCmd( m_part, *node, v, "Modify constraint type" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::constraintStartTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            int c = node->constraint();
            if ( ! ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) ) {
                return " "; //HACK to show focus
            }
            return KGlobal::locale()->formatDateTime( node->constraintStartTime().dateTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( ! ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintStartTime().dateTime() );
        }
        case Qt::EditRole: {
            int c = node->constraint();
            if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) {
                return node->constraintStartTime().dateTime();
            }
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setConstraintStartTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new NodeModifyConstraintStartTimeCmd( m_part, *node, value.toDateTime(), "Modify constraint start time" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::constraintEndTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            int c = node->constraint();
            if ( ! ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) ) {
                return " "; //HACK to show focus
            }
            return KGlobal::locale()->formatDateTime( node->constraintEndTime().dateTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( ! ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintEndTime().dateTime() );
        }
        case Qt::EditRole: {
            int c = node->constraint();
            if ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) {
                return node->constraintEndTime().dateTime();
            }
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setConstraintEndTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new NodeModifyConstraintEndTimeCmd( m_part, *node, value.toDateTime(), "Modify constraint end time" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::estimateType( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->effort()->typeToString( true );
            }
            return QString();
        case Role::EnumList: 
            return Effort::typeToStringList( true );
        case Qt::EditRole:
        case Role::EnumListValue: 
            return (int)node->effort()->type();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setEstimateType( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Effort::Type v = Effort::Type( value.toInt() );
            m_part->addCommand( new ModifyEffortTypeCmd( m_part, *node, node->effort()->type(), v, "Modify estimate type" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::estimate( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->effort()->displayUnit();
                QList<double> scales; // TODO: week
                if ( node->effort()->type() == Effort::Type_Effort ) {
                    scales << m_project->standardWorktime()->day();
                    // rest is default
                }
                double v = Effort::scale( node->effort()->expected(), unit, scales );
                //kDebug()<<k_funcinfo<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            }
            break;
        case Qt::EditRole:
            return node->effort()->expected().milliseconds();
        case Role::DurationScales: {
            QVariantList lst; // TODO: week
            if ( node->effort()->type() == Effort::Type_Effort ) {
                lst.append( m_project->standardWorktime()->day() );
            } else {
                lst.append( 24.0 );
            }
            lst << 60.0 << 60.0 << 1000.0;
            return lst;
        }
        case Role::DurationUnit:
            return static_cast<int>( node->effort()->displayUnit() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setEstimate( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Duration d = value.toList()[0].toLongLong();
            Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
            kDebug()<<k_funcinfo<<value.toList()[0].toLongLong()<<", "<<unit<<" -> "<<d.milliseconds()<<endl;
            KMacroCommand *cmd = new KMacroCommand( i18n( "Modify estimate" ) );
            cmd->addCommand( new ModifyEffortCmd( m_part, *node, node->effort()->expected(), d ) );
            cmd->addCommand( new ModifyEffortUnitCmd( m_part, *node, node->effort()->displayUnit(), unit ) );
            m_part->addCommand( cmd );
            return true;
    }
    return false;
}

QVariant NodeItemModel::optimisticRatio( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->effort()->optimisticRatio();
            }
            return QString();
        case Role::Minimum:
            return -99;
        case Role::Maximum:
            return 0;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setOptimisticRatio( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new EffortModifyOptimisticRatioCmd( m_part, *node, node->effort()->optimisticRatio(), value.toInt(), "Modify estimate" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::pessimisticRatio( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->effort()->pessimisticRatio();
            }
            return QString();
        case Role::Minimum:
            return 0;
        case Role::Maximum:
            return INT_MAX;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setPessimisticRatio( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new EffortModifyPessimisticRatioCmd( m_part, *node, node->effort()->pessimisticRatio(), value.toInt(), "Modify estimate" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::riskType( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->effort()->risktypeToString( true );
            }
            return QString();
        case Role::EnumList: 
            return Effort::risktypeToStringList( true );
        case Qt::EditRole:
        case Role::EnumListValue: 
            return (int)node->effort()->risktype();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setRiskType( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Effort::Risktype v = Effort::Risktype( value.toInt() );
            m_part->addCommand( new EffortModifyRiskCmd( m_part, *node, node->effort()->risktype(), v, "Modify Risk Type" ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::runningAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Account *a = node->runningAccount();
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->runningAccount();
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

bool NodeItemModel::setRunningAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            kDebug()<<k_funcinfo<<node->name()<<endl;
            QStringList lst = runningAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->runningAccount();
            if ( old != a ) {
                m_part->addCommand( new NodeModifyRunningAccountCmd( m_part, *node, old, a, i18n( "Modify Running Account" ) ) );
            }
            return true;
    }
    return false;
}

QVariant NodeItemModel::startupAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task  || node->type() == Node::Type_Milestone ) {
                Account *a = node->startupAccount();
                kDebug()<<k_funcinfo<<node->name()<<": "<<a<<endl;
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->startupAccount();
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

bool NodeItemModel::setStartupAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            kDebug()<<k_funcinfo<<node->name()<<endl;
            QStringList lst = startupAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->startupAccount();
            kDebug()<<k_funcinfo<<(value.toInt())<<"; "<<(lst.at( value.toInt()))<<": "<<a<<endl;
            if ( old != a ) {
                m_part->addCommand( new NodeModifyStartupAccountCmd( m_part, *node, old, a, i18n( "Modify Startup Account" ) ) );
            }
            return true;
    }
    return false;
}

QVariant NodeItemModel::startupCost( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                return KGlobal::locale()->formatMoney( node->startupCost() );
            }
            break;
        case Qt::EditRole:
            return KGlobal::locale()->formatMoney( node->startupCost() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setStartupCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            m_part->addCommand( new NodeModifyStartupCostCmd( m_part, *node, v, i18n( "Modify Startup Cost" ) ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::shutdownAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                Account *a = node->shutdownAccount();
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->shutdownAccount();
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

bool NodeItemModel::setShutdownAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            kDebug()<<k_funcinfo<<node->name()<<endl;
            QStringList lst = shutdownAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->shutdownAccount();
            if ( old != a ) {
                m_part->addCommand( new NodeModifyShutdownAccountCmd( m_part, *node, old, a, i18n( "Modify Shutdown Account" ) ) );
            }
            return true;
    }
    return false;
}

QVariant NodeItemModel::shutdownCost( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                return KGlobal::locale()->formatMoney( node->shutdownCost() );
            }
            break;
        case Qt::EditRole:
            return KGlobal::locale()->formatMoney( node->shutdownCost() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool NodeItemModel::setShutdownCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            m_part->addCommand( new NodeModifyShutdownCostCmd( m_part, *node, v, i18n( "Modify Shutdown Cost" ) ) );
            return true;
    }
    return false;
}

QVariant NodeItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Node *n = node( index );
    if ( n != 0 ) {
        switch ( index.column() ) {
            case 0: result = name( n, role ); break;
            case 1: result = type( n, role ); break;
            case 2: result = leader( n, role ); break;
            case 3: result = allocation( n, role ); break;
            case 4: result = estimateType( n, role ); break;
            case 5: result = estimate( n, role ); break;
            case 6: result = optimisticRatio( n, role ); break;
            case 7: result = pessimisticRatio( n, role ); break;
            case 8: result = riskType( n, role ); break;
            case 9: result = constraint( n, role ); break;
            case 10: result = constraintStartTime( n, role ); break;
            case 11: result = constraintEndTime( n, role ); break;
            case 12: result = runningAccount( n, role ); break;
            case 13: result = startupAccount( n, role ); break;
            case 14: result = startupCost( n, role ); break;
            case 15: result = shutdownAccount( n, role ); break;
            case 16: result = shutdownCost( n, role ); break;
            case 17: result = description( n, role ); break;
            default:
                kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                return QVariant();
        }
        if ( result.isValid() ) {
            if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
                // HACK to show focus in empty cells
                result = " ";
            }
            return result;
        }
    }
    // define default action
    
    return QVariant();
}

bool NodeItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags(index) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Node *n = node( index );
    switch (index.column()) {
        case 0: return setName( n, value, role );
        case 1: return setType( n, value, role );
        case 2: return setLeader( n, value, role );
        case 3: return setAllocation( n, value, role );
        case 4: return setEstimateType( n, value, role );
        case 5: return setEstimate( n, value, role );
        case 6: return setOptimisticRatio( n, value, role );
        case 7: return setPessimisticRatio( n, value, role );
        case 8: return setRiskType( n, value, role );
        case 9: return setConstraint( n, value, role );
        case 10: return setConstraintStartTime( n, value, role );
        case 11: return setConstraintEndTime( n, value, role );
        case 12: return setRunningAccount( n, value, role );
        case 13: return setStartupAccount( n, value, role );
        case 14: return setStartupCost( n, value, role );
        case 15: return setShutdownAccount( n, value, role );
        case 16: return setShutdownCost( n, value, role );
        case 17: return setDescription( n, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant NodeItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Type" );
                case 2: return i18n( "Responsible" );
                case 3: return i18n( "Allocation" );
                case 4: return i18n( "Estimate Type" );
                case 5: return i18n( "Estimate" );
                case 6: return i18n( "Optimistic" );
                case 7: return i18n( "Pessimistic" );
                case 8: return i18n( "Risk" );
                case 9: return i18n( "Constraint" );
                case 10: return i18n( "Constraint Start" );
                case 11: return i18n( "Constraint End" );
                case 12: return i18n( "Running Account" );
                case 13: return i18n( "Startup Account" );
                case 14: return i18n( "Startup Cost" );
                case 15: return i18n( "Shutdown Account" );
                case 16: return i18n( "Shutdown Cost" );
                case 17: return i18n( "Description" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 1: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::NodeName;
            case 1: return ToolTip::NodeType;
            case 2: return ToolTip::NodeResponsible;
            case 3: return ToolTip::Allocation;
            case 4: return ToolTip::EstimateType;
            case 5: return ToolTip::Estimate;
            case 6: return ToolTip::OptimisticRatio;
            case 7: return ToolTip::PessimisticRatio;
            case 8: return ToolTip::RiskType;
            case 9: return ToolTip::NodeConstraint;
            case 10: return ToolTip::NodeConstraintStart;
            case 11: return ToolTip::NodeConstraintEnd;
            case 12: return ToolTip::NodeRunningAccount;
            case 13: return ToolTip::NodeStartupAccount;
            case 14: return ToolTip::NodeStartupCost;
            case 15: return ToolTip::NodeShutdownAccount;
            case 16: return ToolTip::NodeShutdownCost;
            case 17: return ToolTip::NodeDescription;
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QItemDelegate *NodeItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        //case 3: return new ??Delegate( parent );
        case 4: return new EnumDelegate( parent );
        case 5: return new DurationSpinBoxDelegate( parent );
        case 6: return new SpinBoxDelegate( parent );
        case 7: return new SpinBoxDelegate( parent );
        case 8: return new EnumDelegate( parent );
        case 9: return new EnumDelegate( parent );
        case 12: return new EnumDelegate( parent );
        case 13: return new EnumDelegate( parent );
        case 14: return new MoneyDelegate( parent );
        case 15: return new EnumDelegate( parent );
        case 16: return new MoneyDelegate( parent );
        default: return 0;
    }
    return 0;
}

int NodeItemModel::columnCount( const QModelIndex &parent ) const
{
    return 18;
}

int NodeItemModel::rowCount( const QModelIndex &parent ) const
{
    Node *p = node( parent );
    return p->numChildren();
}

void NodeItemModel::sort( int column, Qt::SortOrder order )
{
}

Qt::DropActions NodeItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList NodeItemModel::mimeTypes() const
{
    return QStringList() << "application/x-vnd.kde.kplato.nodeitemmodel.internal";
}

QMimeData *NodeItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (QModelIndex index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            kDebug()<<k_funcinfo<<index.row()<<endl;
            Node *n = node( index );
            if ( n ) {
                rows << index.row();
                stream << n->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.nodeitemmodel.internal", encodedData);
    return m;
}

bool NodeItemModel::dropAllowed( Node *on, const QMimeData *data )
{
    if ( !data->hasFormat("application/x-vnd.kde.kplato.nodeitemmodel.internal") ) {
        return false;
    }
    if ( on == m_project ) {
        return true;
    }
    QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Node*> lst = nodeList( stream );
    foreach ( Node *n, lst ) {
        if ( on == n || on->isChildOf( n ) ) {
            return false;
        }
    }
    lst = removeChildNodes( lst );
    foreach ( Node *n, lst ) {
        if ( ! m_project->canMoveTask( n, on ) ) {
            return false;
        }
    }
    return true;
}

QList<Node*> NodeItemModel::nodeList( QDataStream &stream )
{
    QList<Node*> lst;
    while (!stream.atEnd()) {
        QString id;
        stream >> id;
        Node *node = m_project->findNode( id );
        if ( node ) {
            lst << node;
        }
    }
    return lst;
}

QList<Node*> NodeItemModel::removeChildNodes( QList<Node*> nodes )
{
    QList<Node*> lst;
    foreach ( Node *node, nodes ) {
        bool ins = true;
        foreach ( Node *n, lst ) {
            if ( node->isChildOf( n ) ) {
                kDebug()<<k_funcinfo<<node->name()<<" is child of "<<n->name()<<endl;
                ins = false;
                break;
            }
        }
        if ( ins ) {
            kDebug()<<k_funcinfo<<" insert "<<node->name()<<endl;
            lst << node;
        }
    }
    QList<Node*> nl = lst;
    QList<Node*> nlst = lst;
    foreach ( Node *node, nl ) {
        foreach ( Node *n, nlst ) {
            if ( n->isChildOf( node ) ) {
                kDebug()<<k_funcinfo<<n->name()<<" is child of "<<node->name()<<endl;
                int i = nodes.indexOf( n );
                lst.removeAt( i );
            }
        }
    }
    return lst;
}

bool NodeItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent )
{
    kDebug()<<k_funcinfo<<action<<endl;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.nodeitemmodel.internal" ) ) {
        return false;
    }
    if ( action == Qt::MoveAction ) {
        kDebug()<<k_funcinfo<<"MoveAction"<<endl;
        
        QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        Node *par = 0;
        if ( parent.isValid() ) {
            par = node( parent );
        } else {
            par = m_project;
        }
        QList<Node*> lst = nodeList( stream );
        QList<Node*> nodes = removeChildNodes( lst ); // children goes with their parent
        foreach ( Node *n, nodes ) {
            if ( ! m_project->canMoveTask( n, par ) ) {
                //kDebug()<<k_funcinfo<<"Can't move task: "<<n->name()<<endl;
                return false;
            }
        }
        int offset = 0;
        KMacroCommand *cmd = 0;
        foreach ( Node *n, nodes ) {
            if ( cmd == 0 ) cmd = new KMacroCommand( i18n( "Move tasks" ) );
            cmd->addCommand( new NodeMoveCmd( m_part, m_project, n, par, row + offset ) );
            offset++;
        }
        if ( cmd ) {
            m_part->addCommand( cmd );
        }
        //kDebug()<<k_funcinfo<<row<<", "<<column<<" parent="<<parent.row()<<", "<<parent.column()<<": "<<par->name()<<endl;
        return true;
    }
    return false;
}

Node *NodeItemModel::node( const QModelIndex &index ) const
{
    Node *n = m_project;
    if ( index.isValid() ) {
        if ( index.internalPointer() == 0 ) {
            
        }
        //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
        n = static_cast<Node*>( index.internalPointer() );
        Q_ASSERT( n );
    }
    return n;
}

void NodeItemModel::slotNodeChanged( Node *node )
{
    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->parentNode()->findChildNode( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );
}

QModelIndex NodeItemModel::insertTask( Node *node, Node *after )
{
    m_part->addCommand( new TaskAddCmd( m_part, m_project, node, after, i18n( "Add Task") ) );
    int row = -1;
    if ( node->parentNode() ) {
        row = node->parentNode()->indexOf( node );
    }
    if ( row != -1 ) {
        //kDebug()<<k_funcinfo<<"Inserted: "<<account->name()<<endl;
        return createIndex( row, 0, node );
    }
    kDebug()<<k_funcinfo<<"Can't find "<<node->name()<<endl;
    return QModelIndex();
}

QModelIndex NodeItemModel::insertSubtask( Node *node, Node *parent )
{
    m_part->addCommand( new SubtaskAddCmd( m_part, m_project, node, parent, i18n( "Add Subtask" ) ) );
    int row = -1;
    if ( node->parentNode() ) {
        row = node->parentNode()->indexOf( node );
    }
    if ( row != -1 ) {
        //kDebug()<<k_funcinfo<<"Inserted: "<<account->name()<<endl;
        return createIndex( row, 0, node );
    }
    kDebug()<<k_funcinfo<<"Can't find "<<node->name()<<endl;
    return QModelIndex();
}

//--------------------
NodeTreeView::NodeTreeView( Part *part, QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new NodeItemModel( part ) );
    setSelectionModel( new QItemSelectionModel( model() ) );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    
    for ( int c = 0; c < itemModel()->columnCount(); ++c ) {
        QItemDelegate *delegate = itemModel()->createDelegate( c, this );
        if ( delegate ) {
            setItemDelegateForColumn( c, delegate );
        }
    }
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

}

void NodeTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void NodeTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void NodeTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    foreach( QModelIndex i, selectionModel()->selectedIndexes() ) {
        kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
    }
    TreeViewBase::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void NodeTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    kDebug()<<k_funcinfo<<endl;
    TreeViewBase::currentChanged( current, previous );
    emit currentChanged( current );
}


void NodeTreeView::contextMenuEvent ( QContextMenuEvent * event )
{
    kDebug()<<k_funcinfo<<endl;
    QModelIndex i = indexAt( event->pos() );
    if ( ! i.isValid() ) {
        return;
    }
    Node *node = itemModel()->node( i );
    if ( node == 0 ) {
        return;
    }
    emit contextMenuRequested( node, event->globalPos() );
}

void NodeTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
        return;

    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    //QTreeView thinks it's ok to drop
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Node *dn = itemModel()->node( index );
    if ( dn == 0 ) {
        kError()<<k_funcinfo<<"no node to drop on!"<<endl;
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            //dn == sibling
            if ( itemModel()->dropAllowed( dn->parentNode(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            //dn == new parent
            if ( itemModel()->dropAllowed( dn, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }
}

//-----------------------------------
TaskEditor::TaskEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new NodeTreeView( part, this );
    l->addWidget( m_view );
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    m_view->setDragDropMode( QAbstractItemView::InternalMove );
    m_view->setDropIndicatorShown ( true );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( true );
    
    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT ( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( Node*, const QPoint& ) ), SLOT( slotContextMenuRequested( Node*, const QPoint& ) ) );
}

void TaskEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void TaskEditor::draw()
{
}

void TaskEditor::setGuiActive( bool activate )
{
    kDebug()<<k_funcinfo<<activate<<endl;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskEditor::slotCurrentChanged(  const QModelIndex &curr )
{
    kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
    slotEnableActions();
}

void TaskEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<k_funcinfo<<list.count()<<endl;
    slotEnableActions();
}

int TaskEditor::selectedNodeCount() const
{
    QItemSelectionModel* sm = m_view->selectionModel();
    return sm->selectedRows().count();
}

QList<Node*> TaskEditor::selectedNodes() const {
    QList<Node*> lst;
    QItemSelectionModel* sm = m_view->selectionModel();
    if ( sm == 0 ) {
        return lst;
    }
    foreach ( QModelIndex i, sm->selectedRows() ) {
        Node * n = m_view->itemModel()->node( i );
        if ( n != 0 && n->type() != Node::Type_Project ) {
            lst.append( n );
        }
    }
    return lst;
}

Node *TaskEditor::selectedNode() const
{
    QList<Node*> lst = selectedNodes();
    if ( lst.count() != 1 ) {
        return 0;
    }
    return lst.first();
}

Node *TaskEditor::currentNode() const {
    Node * n = m_view->itemModel()->node( m_view->currentIndex() );
    if ( n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskEditor::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<node->name()<<" : "<<pos<<endl;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Task:
        case Node::Type_Milestone:
            name = "task_popup";
            break;
        case Node::Type_Summarytask:
            name = "summarytask_popup";
            break;
        default:
            name = "node_popup";
    }
    kDebug()<<k_funcinfo<<name<<endl;
    emit requestPopupMenu( name, pos );
}

void TaskEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskEditor::updateActionsEnabled( bool on )
{
    Project *p = m_view->project();
    
    bool o = ( on && p && selectedNodeCount() <= 1 );
    actionAddTask->setEnabled( o );
    actionAddMilestone->setEnabled( o );
    
    actionDeleteTask->setEnabled( on && p && selectedNodeCount() > 0 );
    
    o = ( on && p && selectedNodeCount() == 1 );
    Node *n = selectedNode();
    
    actionAddSubtask->setEnabled( o );
    actionMoveTaskUp->setEnabled( o && p->canMoveTaskUp( n ) );
    actionMoveTaskDown->setEnabled( o && p->canMoveTaskDown( n ) );
    actionIndentTask->setEnabled( o && p->canIndentTask( n ) );
    actionUnindentTask->setEnabled( o && p->canUnindentTask( n ) );
}

void TaskEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    
    QString name = "taskeditor_add_list";
    actionAddTask  = new KAction(KIcon( "add_task" ), i18n("Add Task..."), this);
    actionCollection()->addAction("add_task", actionAddTask );
    connect( actionAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    addAction( name, actionAddTask );
    
    actionAddSubtask  = new KAction(KIcon( "add_sub_task" ), i18n("Add Sub-Task..."), this);
    actionCollection()->addAction("add_sub_task", actionAddSubtask );
    connect( actionAddSubtask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    addAction( name, actionAddSubtask );
    
    actionAddMilestone  = new KAction(KIcon( "add_milestone" ), i18n("Add Milestone..."), this);
    actionCollection()->addAction("add_milestone", actionAddMilestone );
    connect( actionAddMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddMilestone() ) );
    addAction( name, actionAddMilestone );
    
    actionDeleteTask  = new KAction(KIcon( "edit-delete" ), i18n("Delete Task"), this);
    actionCollection()->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );
    addAction( name, actionDeleteTask );

    
    name = "taskeditor_move_list";
    actionIndentTask  = new KAction(KIcon("indent_task"), i18n("Indent Task"), this);
    actionCollection()->addAction("indent_task", actionIndentTask );
    connect(actionIndentTask, SIGNAL(triggered(bool) ), SLOT(slotIndentTask()));
    addAction( name, actionIndentTask );
    
    actionUnindentTask  = new KAction(KIcon("unindent_task"), i18n("Unindent Task"), this);
    actionCollection()->addAction("unindent_task", actionUnindentTask );
    connect(actionUnindentTask, SIGNAL(triggered(bool) ), SLOT(slotUnindentTask()));
    addAction( name, actionUnindentTask );
    
    actionMoveTaskUp  = new KAction(KIcon("move_task_up"), i18n("Move Up"), this);
    actionCollection()->addAction("move_task_up", actionMoveTaskUp );
    connect(actionMoveTaskUp, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskUp()));
    addAction( name, actionMoveTaskUp );
    
    actionMoveTaskDown  = new KAction(KIcon("move_task_down"), i18n("Move Down"), this);
    actionCollection()->addAction("move_task_down", actionMoveTaskDown );
    connect(actionMoveTaskDown, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskDown()));
    addAction( name, actionMoveTaskDown );

}

void TaskEditor::slotAddTask()
{
    kDebug()<<k_funcinfo<<endl;
    if ( selectedNodeCount() == 0 ) {
        // insert under main project
        Task *t = m_view->project()->createTask( part()->config().taskDefaults(), m_view->project() );
        edit( m_view->itemModel()->insertSubtask( t, t->parentNode() ) );
        return;
    }
    Node *sib = selectedNode();
    if ( sib == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( part()->config().taskDefaults(), sib->parentNode() );
    edit( m_view->itemModel()->insertTask( t, sib ) );
}

void TaskEditor::slotAddMilestone()
{
    kDebug()<<k_funcinfo<<endl;
    if ( selectedNodeCount() == 0 ) {
        // insert under main project
        Task *t = m_view->project()->createTask( part()->config().taskDefaults(), m_view->project() );
        t->effort()->set( Duration::zeroDuration );
        edit( m_view->itemModel()->insertSubtask( t, t->parentNode() ) );
        return;
    }
    Node *sib = selectedNode(); // sibling
    if ( sib == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( part()->config().taskDefaults(), sib->parentNode() );
    t->effort()->set( Duration::zeroDuration );
    edit( m_view->itemModel()->insertTask( t, sib ) );
}

void TaskEditor::slotAddSubtask()
{
    kDebug()<<k_funcinfo<<endl;
    Node *parent = selectedNode();
    if ( parent == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( part()->config().taskDefaults(), parent );
    edit( m_view->itemModel()->insertSubtask( t, parent ) );
}

void TaskEditor::edit( QModelIndex i )
{
    if ( i.isValid() ) {
        QModelIndex p = m_view->model()->parent( i );
        m_view->setExpanded( p, true );
        m_view->setCurrentIndex( i );
        m_view->edit( i );
    }
}

void TaskEditor::slotDeleteTask()
{
    //kDebug()<<k_funcinfo<<endl;
    QList<Node*> lst = selectedNodes();
    while ( true ) {
        // remove children of selected tasks, as parents delete their children
        Node *ch = 0;
        foreach ( Node *n1, lst ) {
            foreach ( Node *n2, lst ) {
                if ( n2->isChildOf( n1 ) ) {
                    ch = n2;
                    break;
                }
            }
            if ( ch != 0 ) {
                break;
            }
        }
        if ( ch == 0 ) {
            break;
        }
        lst.removeAt( lst.indexOf( ch ) );
    }
    //foreach ( Node* n, lst ) { kDebug()<<k_funcinfo<<n->name()<<endl; }
    emit deleteTaskList( lst );
}

void TaskEditor::slotIndentTask()
{
    kDebug()<<k_funcinfo<<endl;
    emit indentTask();
}

void TaskEditor::slotUnindentTask()
{
    kDebug()<<k_funcinfo<<endl;
    emit unindentTask();
}

void TaskEditor::slotMoveTaskUp()
{
    kDebug()<<k_funcinfo<<endl;
    emit moveTaskUp();
}

void TaskEditor::slotMoveTaskDown()
{
    kDebug()<<k_funcinfo<<endl;
    emit moveTaskDown();
}


} // namespace KPlato

#include "kpttaskeditor.moc"
