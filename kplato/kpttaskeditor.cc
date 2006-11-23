/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

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
#include <QHeaderView>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstdaction.h>
#include <kstdaccel.h>
#include <kaccelgen.h>

namespace KPlato
{

NodeItemModel::NodeItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent )
{
}

NodeItemModel::~NodeItemModel()
{
}
    
void NodeItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotLayoutChanged() ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    m_project = project;
    connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    
    connect( m_project, SIGNAL( nodeToBeAdded( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    connect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
    connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
}
    
Qt::ItemFlags NodeItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        flags &= ~Qt::ItemIsEditable;
    } else {
        switch ( index.column() ) {
            case 1: break; // Node type
            case 4: { // constraint start
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case 5: { // constraint end
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustFinishOn || c == Node::FinishNotLater || c ==  Node::FixedInterval ) {
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
    Node *p = node( index )->getParent();
    if ( p == 0 || p->type() == Node::Type_Project ) {
        return QModelIndex();
    }
    int row = p->getParent()->findChildNode( p );
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

int NodeItemModel::columnCount( const QModelIndex &parent ) const
{
    return 8;
}

int NodeItemModel::rowCount( const QModelIndex &parent ) const
{
    Node *p = node( parent );
    return p->numChildren();
}

bool NodeItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
    //TODO
    return false;
}

bool NodeItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
    //TODO
    return false;
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
        case Qt::EditRole: 
        case Qt::ToolTipRole:
            return node->constraintToString( true );
        case Role::EnumList: 
            return Node::constraintList( true );
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
            return KGlobal::locale()->formatDateTime( node->constraintStartTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( ! ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintStartTime() );
        }
        case Qt::EditRole: {
            int c = node->constraint();
            if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) {
                return node->constraintStartTime();
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
            return KGlobal::locale()->formatDateTime( node->constraintEndTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( ! ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintEndTime() );
        }
        case Qt::EditRole: {
            int c = node->constraint();
            if ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) {
                return node->constraintEndTime();
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
        case Qt::EditRole: 
        case Qt::ToolTipRole:
            return node->effort()->typeToString( true );
        case Role::EnumList: 
            return Effort::typeToStringList( true );
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

QVariant NodeItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Node *n = node( index );
    if ( n != 0 ) {
        switch ( index.column() ) {
            case 0: result = name( n, role ); break;
            case 1: result = type( n, role ); break;
            case 2: result = leader( n, role ); break;
            case 3: result = constraint( n, role ); break;
            case 4: result = constraintStartTime( n, role ); break;
            case 5: result = constraintEndTime( n, role ); break;
            case 6: result = estimateType( n, role ); break;
            case 7: result = description( n, role ); break;
            default:
                kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                return QVariant();
        }
        if ( result.isValid() ) {
            if ( result.type() == QVariant::String && result.toString().isEmpty()) {
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
        case 3: return setConstraint( n, value, role );
        case 4: return setConstraintStartTime( n, value, role );
        case 5: return setConstraintEndTime( n, value, role );
        case 6: return setEstimateType( n, value, role );
        case 7: return setDescription( n, value, role );
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
                case 3: return i18n( "Constraint" );
                case 4: return i18n( "Constraint Start" );
                case 5: return i18n( "Constraint End" );
                case 6: return i18n( "Estimate" );
                case 7: return i18n( "Description" );
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
            case 3: return ToolTip::NodeConstraint;
            case 4: return ToolTip::NodeConstraintStart;
            case 5: return ToolTip::NodeConstraintEnd;
            case 6: return ToolTip::NodeEstimateType;
            case 7: return ToolTip::NodeDescription;
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void NodeItemModel::sort( int column, Qt::SortOrder order )
{
}

QMimeData * NodeItemModel::mimeData( const QModelIndexList &indexes ) const
{
    return 0;
}

QStringList NodeItemModel::mimeTypes () const
{
    return QStringList();
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
    int row = node->getParent()->findChildNode( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );
}

//--------------------
NodeTreeView::NodeTreeView( Part *part, QWidget *parent )
    : QTreeView( parent ),
    m_arrowKeyNavigation( true )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new NodeItemModel( part ) );
    setSelectionModel( new QItemSelectionModel( model() ) );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    
    setItemDelegateForColumn( 3, new EnumDelegate( this ) );
    setItemDelegateForColumn( 6, new EnumDelegate( this ) );

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
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void NodeTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    kDebug()<<k_funcinfo<<endl;
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
}

/*!
  \reimp
 */
void NodeTreeView::keyPressEvent(QKeyEvent *event)
{
    kDebug()<<k_funcinfo<<event->key()<<", "<<m_arrowKeyNavigation<<endl;
    if ( !m_arrowKeyNavigation ) {
        QTreeView::keyPressEvent( event );
        return;
    }
    QModelIndex current = currentIndex();
    if ( current.isValid() ) {
        switch (event->key()) {
            case Qt::Key_Right: {
                if ( current.column() < model()->columnCount() - 1 ) {
                    QModelIndex i = model()->index( current.row(), current.column() + 1, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Left: {
                if ( current.column() > 0 ) {
                    QModelIndex i = model()->index( current.row(), current.column() - 1, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Down: {
                QModelIndex i = indexBelow( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Up: {
                QModelIndex i = indexAbove( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Plus:
                if ( itemsExpandable()) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                        //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
            case Qt::Key_Minus:
                if ( itemsExpandable() ) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                        //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
        }
    }
    QTreeView::keyPressEvent(event);
}

QItemSelectionModel::SelectionFlags NodeTreeView::selectionCommand(const QModelIndex &index,
        const QEvent *event) const
{
/*    if ( event && event->type() == QEvent::KeyPress && selectionMode() == QAbstractItemView::ExtendedSelection ) {
        if ( static_cast<const QKeyEvent*>(event)->key() == Qt::Key_Space ) {
            Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
            QItemSelectionModel::SelectionFlags bflags = QItemSelectionModel::Rows;
            // 
            if ( modifiers && Qt::ShiftModifier ) {
                return QItemSelectionModel::SelectCurrent|bflags;
            }
            // Toggle on Ctrl-Qt::Key_Space
            if ( modifiers & Qt::ControlModifier ) {
                return QItemSelectionModel::Toggle|bflags;
            }
            // Select on Space alone
            return QItemSelectionModel::ClearAndSelect|bflags;
        }
    }*/
    return QTreeView::selectionCommand( index, event );
}

//-----------------------------------
TaskEditor::TaskEditor( View *view, QWidget *parent )
    : ViewBase( view, parent )
{
    setInstance(Factory::global());
    if ( view->getPart()->isReadWrite() ) {
        setXMLFile("kplato_taskeditor.rc");
    }
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_editor = new NodeTreeView( view->getPart(), this );
    l->addWidget( m_editor );
    m_editor->setEditTriggers( m_editor->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( m_editor, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT ( slotCurrentChanged( QModelIndex ) ) );

    connect( m_editor, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );
}

void TaskEditor::draw( Project &project )
{
    m_editor->setProject( &project );
}

void TaskEditor::draw()
{
}

void TaskEditor::setViewActive( bool activate, KXMLGUIFactory *factory ) // slot
{
    kDebug()<<k_funcinfo<<activate<<endl;
    if ( activate ) {
        addActions( factory );
        updateActionsEnabled( true );
    } else {
        removeActions();
    }
    if ( activate && !m_editor->currentIndex().isValid() ) {
        m_editor->selectionModel()->setCurrentIndex(m_editor->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
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
    QItemSelectionModel* sm = m_editor->selectionModel();
    return sm->selectedRows().count();
}

QList<Node*> TaskEditor::selectedNodes() const {
    QList<Node*> lst;
    QItemSelectionModel* sm = m_editor->selectionModel();
    if ( sm == 0 ) {
        return lst;
    }
    foreach ( QModelIndex i, sm->selectedRows() ) {
        Node * n = m_editor->itemModel()->node( i );
        if ( n != 0 && n->type() != Node::Type_Project ) {
            lst.append( n );
        }
    }
    return lst;
}

Node *TaskEditor::selectedNode() const
{
    QItemSelectionModel* sm = m_editor->selectionModel();
    if ( sm == 0 || sm->selectedRows().count() != 1 ) {
        return 0;
    }
    Node * n = m_editor->itemModel()->node( sm->selectedRows().first() );
    if ( n->type() == Node::Type_Project ) {
        return 0;
    }
    kDebug()<<k_funcinfo<<n->name()<<endl;
    return n;
}

Node *TaskEditor::currentNode() const {
    Node * n = m_editor->itemModel()->node( m_editor->currentIndex() );
    if ( n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskEditor::updateActionsEnabled( bool on )
{
    Project *p = m_editor->project();
    
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
    actionAddTask = new KAction( KIcon( "add_task" ), i18n( "Add Task..." ), actionCollection(), "add_task" );
    connect( actionAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    actionAddSubtask = new KAction( KIcon( "add_sub_task" ), i18n( "Add Sub-Task..." ), actionCollection(), "add_sub_task" );
    connect( actionAddSubtask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    actionAddMilestone = new KAction( KIcon( "add_milestone" ), i18n( "Add Milestone..." ), actionCollection(), "add_milestone" );
    connect( actionAddMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddMilestone() ) );
    actionDeleteTask = new KAction( KIcon( "editdelete" ), i18n( "Delete Task" ), actionCollection(), "delete_task" );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );

    
    actionIndentTask = new KAction(KIcon("indent_task"), i18n("Indent Task"), actionCollection(), "indent_task");
    connect(actionIndentTask, SIGNAL(triggered(bool) ), SLOT(slotIndentTask()));
    actionUnindentTask = new KAction(KIcon("unindent_task"), i18n("Unindent Task"), actionCollection(), "unindent_task");
    connect(actionUnindentTask, SIGNAL(triggered(bool) ), SLOT(slotUnindentTask()));
    actionMoveTaskUp = new KAction(KIcon("move_task_up"), i18n("Move Up"), actionCollection(), "move_task_up");
    connect(actionMoveTaskUp, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskUp()));
    actionMoveTaskDown = new KAction(KIcon("move_task_down"), i18n("Move Down"), actionCollection(), "move_task_down");
    connect(actionMoveTaskDown, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskDown()));

}

void TaskEditor::slotAddTask()
{
    kDebug()<<k_funcinfo<<endl;
    emit addTask();
}

void TaskEditor::slotAddMilestone()
{
    kDebug()<<k_funcinfo<<endl;
    emit addMilestone();
}

void TaskEditor::slotAddSubtask()
{
    kDebug()<<k_funcinfo<<endl;
    emit addSubtask();
}

void TaskEditor::slotDeleteTask()
{
    kDebug()<<k_funcinfo<<endl;
    emit deleteTaskList( selectedNodes() );
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
