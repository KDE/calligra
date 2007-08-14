/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
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
#include "kptpertresult.h"
#include "kptitemmodelbase.h"
#include "kptpart.h"
#include "kpttask.h"
#include "kptnode.h"
#include "kptschedule.h"

#include <QTreeView>
#include <QStringList>

#include <klocale.h>

namespace KPlato
{
  
class Project;
class Node;
class Task;

typedef QList<Node*> NodeList;

PertResultItemModel::PertResultItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_manager( 0 )
{
/*    connect( this, SIGNAL( modelAboutToBeReset() ), SLOT( slotAboutToBeReset() ) );
    connect( this, SIGNAL( modelReset() ), SLOT( slotReset() ) );*/
}

PertResultItemModel::~PertResultItemModel()
{
}
    
void PertResultItemModel::slotAboutToBeReset()
{
    kDebug()<<k_funcinfo<<endl;
    clear();
}

void PertResultItemModel::slotReset()
{
    kDebug()<<k_funcinfo<<endl;
    refresh();
}

void PertResultItemModel::slotNodeToBeInserted( Node *, int )
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    clear();
}

void PertResultItemModel::slotNodeInserted( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->getParent->name()<<"-->"<<node->name()<<endl;
    refresh();
}

void PertResultItemModel::slotNodeToBeRemoved( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    clear();
}

void PertResultItemModel::slotNodeRemoved( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    refresh();
}

void PertResultItemModel::setProject( Project *project )
{
    clear();
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
    refresh();
}

void PertResultItemModel::setManager( ScheduleManager *sm )
{
    m_manager = sm;
    refresh();
}

void PertResultItemModel::clear()
{
    foreach ( NodeList *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            // FIXME: gives error msg:
            // Can't select indexes from different model or with different parents
            QModelIndex i = index( l );
            kDebug()<<k_funcinfo<<i<<": "<<c<<endl;
//            beginRemoveRows( i, 0, c-1 );
//            endRemoveRows();
        }
    }
    m_critical.clear();
    m_noncritical.clear();
    if ( ! m_top.isEmpty() ) {
        beginRemoveRows( QModelIndex(), 0, m_top.count() - 1 );
        m_top.clear();
        m_topNames.clear();
        endRemoveRows();
    }
}

void PertResultItemModel::refresh()
{
    clear();
    if ( m_project == 0 ) {
        return;
    }
    long id = m_manager == 0 ? -1 : m_manager->id();
    kDebug()<<k_funcinfo<<id<<endl;
    if ( id == -1 ) {
        return;
    }
    m_topNames << i18n( "Project" );
    m_top << &m_dummyList; // dummy
    const QList< NodeList > *lst = m_project->criticalPathList( id );
    if ( lst ) {
        for ( int i = 0; i < lst->count(); ++i ) {
            m_topNames << i18n( "Critical Path" );
            m_top.append( const_cast<NodeList*>( &( lst->at( i ) ) ) );
            kDebug()<<k_funcinfo<<m_topNames.last()<<lst->at( i )<<endl;
        }
        if ( lst->isEmpty() ) kDebug()<<k_funcinfo<<"No critical path"<<endl;
    }
    foreach( Node* n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        if ( t->inCriticalPath( id ) ) {
            continue;
        } else if ( t->isCritical( id ) ) {
            m_critical.append( t );
        } else {
            m_noncritical.append( t );
        }
    }
    if ( ! m_critical.isEmpty() ) {
        m_topNames << i18n( "Critical" );
        m_top.append(&m_critical );
    }
    if ( ! m_noncritical.isEmpty() ) {
        m_topNames << i18n( "Non-critical" );
        m_top.append(&m_noncritical );
    }
    if ( ! m_top.isEmpty() ) {
        kDebug()<<k_funcinfo<<m_top<<endl;
        beginInsertRows( QModelIndex(), 0, m_top.count() -1 );
        endInsertRows();
        foreach ( NodeList *l, m_top ) {
            int c = l->count();
            if ( c > 0 ) {
                beginInsertRows( index( l ), 0, c-1 );
                endInsertRows();
            }
        }
    }
}

Qt::ItemFlags PertResultItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    flags &= ~( Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    return flags;
}

    
QModelIndex PertResultItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
    int row = index.internalId();
    if ( row < 0 ) {
        return QModelIndex(); // top level has no parent
    }
    if ( m_top.value( row ) == 0 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, -1 );
}

QModelIndex PertResultItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row == 0 ) {
            QModelIndex idx = createIndex(row, column, -2 ); // project
            return idx;
        }
        if ( row >= m_top.count() ) {
            return QModelIndex(); // shouldn't happend
        }
        QModelIndex idx = createIndex(row, column, -1 );
        //kDebug()<<k_funcinfo<<parent<<", "<<idx<<endl;
        return idx;
    }
    if ( parent.row() == 0 ) {
        return QModelIndex();
    }
    NodeList *l = m_top.value( parent.row() );
    if ( l == 0 ) {
        return QModelIndex();
    }
    QModelIndex i = createIndex(row, column, parent.row() );
    return i;
}

// QModelIndex PertResultItemModel::index( const Node *node ) const
// {
//     if ( m_project == 0 || node == 0 ) {
//         return QModelIndex();
//     }
//     foreach( NodeList *l, m_top ) {
//         int row = l->indexOf( const_cast<Node*>( node ) );
//         if ( row != -1 ) {
//             return createIndex( row, 0, const_cast<Node*>( node ) );
//         }
//     }
//     return QModelIndex();
// }

QModelIndex PertResultItemModel::index( const NodeList *lst ) const
{
    if ( m_project == 0 || lst == 0 ) {
        return QModelIndex();
    }
    NodeList *l = const_cast<NodeList*>( lst );
    int row = m_top.indexOf( l );
    if ( row <= 0 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, -1 );
}

QVariant PertResultItemModel::name( int row, int role ) const
{
    
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return m_topNames.value( row );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::name( const Node *node, int role ) const
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

QVariant PertResultItemModel::earlyStart( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->earlyStart( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->earlyStart( m_manager->id() ).date() );
            return QString();
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::earlyFinish( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->earlyFinish( m_manager->id() ).dateTime();
        case Qt::ToolTipRole: {
            return KGlobal::locale()->formatDate( node->earlyFinish( m_manager->id() ).date() );
            break;
        }
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::lateStart( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->lateStart( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->lateStart( m_manager->id() ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::lateFinish( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->lateFinish( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->lateFinish( m_manager->id() ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::positiveFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->positiveFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->positiveFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::freeFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->freeFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->freeFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    if ( ! index.isValid() ) {
        return result;
    }
    if ( role == Qt::TextAlignmentRole ) {
        return alignment( index.column() );
    }
    Node *n = node( index );
    if ( n == 0 ) {
        switch ( index.column() ) {
            case 0: return name( index.row(), role );
            default: break;
        }
        return QVariant();
    }
    if ( n->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( n );
        switch ( index.column() ) {
            case 0: result = name( t, role ); break;
            case 1: result = earlyStart( t, role ); break;
            case 2: result = earlyFinish( t, role ); break;
            case 3: result = lateStart( t, role ); break;
            case 4: result = lateFinish( t, role ); break;
            case 5: result = positiveFloat( t, role ); break;
            case 6: result = freeFloat( t, role ); break;
            default:
                kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                return QVariant();
        }
    }
    if ( n->type() == Node::Type_Project ) {
        Project *p = static_cast<Project*>( n );
        switch ( index.column() ) {
            case 0: result = name( 0, role ); break;
/*            case 1: result = earlyStart( p, role ); break;
            case 2: result = earlyFinish( p, role ); break;
            case 3: result = lateStart( p, role ); break;
            case 4: result = lateFinish( p, role ); break;
            case 5: result = positiveFloat( p, role ); break;
            case 6: result = freeFloat( p, role ); break;*/
            default:
                //kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                return QVariant();
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

bool PertResultItemModel::setData( const QModelIndex &, const QVariant &, int )
{
    return false;
}

QVariant PertResultItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Early Start" );
                case 2: return i18n( "Early Finish" );
                case 3: return i18n( "Late Start" );
                case 4: return i18n( "Late Finish" );
                case 5: return i18n( "Positive Float" );
                case 6: return i18n( "Free Float" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            return alignment( section );
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant PertResultItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default
        default: return Qt::AlignCenter;
    }
    return QVariant();
}

QItemDelegate *PertResultItemModel::createDelegate( int column, QWidget */*parent*/ ) const
{
    switch ( column ) {
        default: return 0;
    }
    return 0;
}

int PertResultItemModel::columnCount( const QModelIndex & ) const
{
    return 7;
}

int PertResultItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        //kDebug()<<k_funcinfo<<"top="<<m_top.count()<<endl;
        return m_top.count();
    }
    NodeList *l = list( parent );
    if ( l ) {
        //kDebug()<<k_funcinfo<<"list "<<parent.row()<<": "<<l->count()<<endl;
        return l->count();
    }
    //kDebug()<<k_funcinfo<<"node "<<parent.row()<<endl;
    return 0; // nodes don't have children
}

Qt::DropActions PertResultItemModel::supportedDropActions() const
{
    return (Qt::DropActions)Qt::CopyAction | Qt::MoveAction;
}


QStringList PertResultItemModel::mimeTypes() const
{
    return QStringList();
}

QMimeData *PertResultItemModel::mimeData( const QModelIndexList & ) const
{
    QMimeData *m = new QMimeData();
    return m;
}

bool PertResultItemModel::dropAllowed( Node *, const QMimeData * )
{
    return false;
}

bool PertResultItemModel::dropMimeData( const QMimeData *, Qt::DropAction , int , int , const QModelIndex & )
{
    return false;
}

NodeList *PertResultItemModel::list( const QModelIndex &index ) const
{
    if ( index.isValid() && index.internalId() == -1 ) {
        //kDebug()<<k_funcinfo<<index<<"is list: "<<m_top.value( index.row() )<<endl;
        return m_top.value( index.row() );
    }
    //kDebug()<<k_funcinfo<<index<<"is not list"<<endl;
    return 0;
}

Node *PertResultItemModel::node( const QModelIndex &index ) const
{
    if ( ! index.isValid() ) {
        return 0;
    }
    if ( index.internalId() == -2 ) {
        return m_project;
    }
    if ( index.internalId() == 0 ) {
        return 0;
    }
    NodeList *l = m_top.value( index.internalId() );
    if ( l ) {
        return l->value( index.row() );
    }
    return 0;
}

void PertResultItemModel::slotNodeChanged( Node *)
{
    kDebug()<<k_funcinfo<<endl;
    refresh();
/*    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->getParent()->findChildNode( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );*/
}

//-----------------------------------
PertResult::PertResult( Part *part, QWidget *parent ) : ViewBase( part, parent )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------" << endl;
    //widget.setupUi(this);
    PertResultItemModel *m = new PertResultItemModel( part );
/*    widget.treeWidgetTaskResult->setModel( m );
    widget.treeWidgetTaskResult->header()->setStretchLastSection( false );*/
    treeWidgetTaskResult = new TreeViewBase( this );
    treeWidgetTaskResult->setModel( m );
    treeWidgetTaskResult->header()->setStretchLastSection( false );

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( treeWidgetTaskResult );
//    QHeaderView *header=widget.treeWidgetTaskResult->header();
    
    current_schedule=0;
    m_part = part;
    m_project = &m_part->getProject();
    m_node = m_project;

        
/*    (*header).resizeSection(0,60);
    (*header).resizeSection(1,120);
    (*header).resizeSection(2,110);
    (*header).resizeSection(3,110);
    (*header).resizeSection(4,110);
    (*header).resizeSection(5,110);
    (*header).resizeSection(6,80);
    (*header).resizeSection(7,80);*/
    draw( part->getProject() );
}

void PertResult::draw( Project &project)
{
    setProject( &project );
//    draw();
}
  
void PertResult::draw()
{
/*    widget.scheduleName->setText( i18n( "None" ) );
    widget.totalFloat->clear();
    if ( m_project && model()->manager() && model()->manager()->isScheduled() ) {
        long id = model()->manager()->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( model()->manager()->name() );
        KLocale *locale =  KGlobal::locale();
        Duration f;
        foreach ( Node *n, m_project->allNodes() ) {
            if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                f += static_cast<Task*>( n )->positiveFloat( id );
            }
        }
        widget.totalFloat->setText( locale->formatNumber( f.toDouble( Duration::Unit_h ) ) );
    }*/
/*    kDebug() << "UPDATE PE" << endl;
      widget.treeWidgetTaskResult->clear();
      if ( current_schedule == 0 || current_schedule->id() == -1 ) {
          return;
    }
    KLocale * locale = KGlobal::locale();
    QList<Node*> list;
    QString res;
    testComplexGraph();
    foreach(Node * currentNode, m_project->childNodeIterator()){
        if (currentNode->type()!=4){
 
            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->id());
            item->setText(1, currentNode->name());
            item->setText(2,locale->formatDateTime(getStartEarlyDate(currentNode)));
            item->setText(3,locale->formatDateTime(getFinishEarlyDate(currentNode)));
            item->setText(4,locale->formatDateTime(getStartLateDate(currentNode)));
            item->setText(5,locale->formatDateTime(getFinishLateDate(currentNode)));
            item->setText(6,res.number(getTaskFloat(currentNode).days()));
            item->setText(7,res.number(getFreeMargin(currentNode).days()));
        }
        widget.totalFloat->setText(res.number(getProjectFloat(m_project).days()));

    }
    list=criticalPath();
    QList<Node*>::iterator it=list.begin();
    while(it!=list.end()) 
    {
         res+=(*it)->id();
         it++;
         if(it!=list.end()) res+=" - ";
    }
    widget.labelResultCriticalPath->setText(res);*/
}

DateTime PertResult::getStartEarlyDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    //if the task has no parent so the early date start is 0
    duration=t->earlyStart(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

DateTime PertResult::getFinishEarlyDate(Node * currentNode)
{
    //it's the early start date + duration of the task
    Task * t;
    t=static_cast<Task *>(currentNode);
    DateTime duration;

    duration=t->earlyFinish(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);
}
 
DateTime PertResult::getStartLateDate(Node * currentNode)
{
    Task * t;
    DateTime duration;
    t=static_cast<Task *>(currentNode);
    duration=t->lateStart(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);

}


DateTime PertResult::getFinishLateDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    duration=t->lateFinish(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

Duration PertResult::getProjectFloat(Project *project)
{
    Duration duration;
    foreach(Node * currentNode, project->childNodeIterator() )
    {
	duration=duration+getTaskFloat(currentNode);
    }
    //duration.setDayOnly(true);
    return duration;
}

Duration PertResult::getFreeMargin(Node * currentNode)
{
    //search the small duration of the nextest task
    Task * t;
    DateTime duration;
    for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    {
        if(it==currentNode->dependChildNodes().begin())
        {
	    duration=getStartEarlyDate((*it)->child());
	}
	t=static_cast<Task *>((*it)->child ());
        if(getStartEarlyDate((*it)->child ())<duration)
	{
	    duration=getStartEarlyDate((*it)->child ());
	}
    }
    t=static_cast<Task *>(currentNode);
    duration.setDateOnly(true);
    return duration-(getStartEarlyDate(currentNode)+=(t->endTime()-t->startTime())); 
}

Duration PertResult::getTaskFloat(Node * currentNode)
{
    if(currentNode->dependChildNodes().size()==0  && complexGraph==true)
    {
         return getFinishLateDate(currentNode)-getStartEarlyDate(currentNode);
    }
    else
    {
        return getFinishLateDate(currentNode)-getFinishEarlyDate(currentNode);
    }
}

QList<Node*> PertResult::criticalPath()
{
    QList<Node*> list;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependChildNodes().size()==0 && getFinishLateDate(currentNode)==getStartEarlyDate(currentNode))
    	{
          list.push_back(currentNode) ;
    	}
	else
	{
	   if(getFinishLateDate(currentNode)==getFinishEarlyDate(currentNode))
	   {
               list.push_back(currentNode) ;
	   }
	}
    }
    return list;
}
void PertResult::testComplexGraph()
{
    complexGraph=false;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependParentNodes().size()>1)
	{
	    complexGraph=true;
	}
    } 
}

void PertResult::slotUpdate(){

    draw(m_part->getProject());
}

void PertResult::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    kDebug()<<k_funcinfo<<sm;
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertResult::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        //draw();
        model()->setManager( sm );
    }
}

void PertResult::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == model()->manager() ) {
        current_schedule = 0;
        model()->setManager( 0 );
//        draw(); // clears view
    }
}

void PertResult::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    }
    m_project = project;
    treeWidgetTaskResult->itemModel()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    }
    draw();
}


//--------------------
PertCpmView::PertCpmView( Part *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_project( 0 ),
    current_schedule( 0 )
{
    kDebug() << " ---------------- KPlato: Creating PertCpmView ----------------" << endl;
    widget.setupUi(this);

    PertResultItemModel *m = new PertResultItemModel( part );
    widget.cpmTable->setModel( m );
    
    setProject( &( part->getProject() ) );

}

void PertCpmView::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    kDebug()<<k_funcinfo<<sm<<endl;
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertCpmView::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        model()->setManager( sm );
    }
}

void PertCpmView::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == current_schedule ) {
        current_schedule = 0;
        model()->setManager( 0 );
    }
}

void PertCpmView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    }
    m_project = project;
    widget.cpmTable->model()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
    }
    draw();
}

void PertCpmView::draw()
{
    widget.scheduleName->setText( i18n( "None" ) );
    if ( m_project && current_schedule && current_schedule->isScheduled() ) {
        long id = current_schedule->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( current_schedule->name() );
    }
}

void PertCpmView::slotUpdate()
{
    draw();
}


} // namespace KPlato

#include "kptpertresult.moc"
