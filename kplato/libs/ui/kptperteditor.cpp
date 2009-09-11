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

#include "kptperteditor.h"
#include "kptproject.h"
#include "kptrelationeditor.h"

#include <KoDocument.h>

#include <kglobal.h>
#include <klocale.h>

#include <QModelIndex>

namespace KPlato
{

//-----------------------------------
PertEditor::PertEditor( KoDocument *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_project( 0 )
{
    kDebug() <<" ---------------- KPlato: Creating PertEditor ----------------";
    widget.setupUi(this);

    m_tasktree = widget.taskList;
    m_tasktree->setSelectionMode( QAbstractItemView::SingleSelection );
    
    m_availableList = widget.available;
    m_availableList->setSelectionMode( QAbstractItemView::SingleSelection );
    
    m_requiredList = widget.required;
    m_requiredList->hideColumn( 1 ); // child node name
    m_requiredList->setEditTriggers( QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed );
    connect( m_requiredList->model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    updateReadWrite( part->isReadWrite() && ! part->isEmbedded() );
    
    widget.addBtn->setIcon( KIcon( "arrow-right" ) );
    widget.removeBtn->setIcon( KIcon( "arrow-left" ) );
    slotAvailableChanged( 0 );
    slotRequiredChanged( QModelIndex() );
    
    connect( m_tasktree, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ), SLOT( slotCurrentTaskChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ) );
    connect( m_availableList, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ), SLOT( slotAvailableChanged( QTreeWidgetItem * ) ) );
    connect( m_requiredList->selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( slotRequiredChanged( const QModelIndex& ) ) );
    
    connect( widget.addBtn, SIGNAL(clicked()), this, SLOT(slotAddClicked() ) );
    connect( widget.removeBtn, SIGNAL(clicked() ), this, SLOT(slotRemoveClicked() ) );

    connect( this, SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
}

void PertEditor::slotCurrentTaskChanged( QTreeWidgetItem *curr, QTreeWidgetItem *prev )
{
    //kDebug()<<curr<<prev;
    if ( curr == 0 ) {
        m_availableList->clear();
        loadRequiredTasksList( 0 );
    } else if ( prev == 0 ) {
        dispAvailableTasks();
    } else {
        updateAvailableTasks();
        loadRequiredTasksList( itemToNode( curr ) );
    }
    slotAvailableChanged( m_availableList->currentItem() );
}

void PertEditor::slotAvailableChanged( QTreeWidgetItem *item )
{
    //kDebug()<<(item?item->text(0):"nil")<<(item?item->data( 0, EnabledRole ).toBool():false);
    if ( item == 0 || item == m_availableList->currentItem() ) {
        widget.addBtn->setEnabled( item != 0 && item->data( 0, EnabledRole ).toBool() );
    }
}

void PertEditor::slotRequiredChanged( const QModelIndex &item )
{
    //kDebug()<<item;
    widget.removeBtn->setEnabled( item.isValid() );
}

void PertEditor::slotAddClicked()
{
    if ( ! isReadWrite() ) {
        return;
    }
    QTreeWidgetItem *item = m_availableList->currentItem();
    //kDebug()<<item;
    addTaskInRequiredList( item );
    updateAvailableTasks( item );
}

void PertEditor::addTaskInRequiredList(QTreeWidgetItem * currentItem)
{
    //kDebug()<<currentItem;
    if ( currentItem == 0 ) {
        return;
    }
    if ( m_project == 0 ) {
        return;
    }
    // add the relation between the current task and the current task
    QTreeWidgetItem *selectedTask = m_tasktree->currentItem();
    if ( selectedTask == 0 ) {
        return;
    }

    Node *par = itemToNode( currentItem );
    Node *child = itemToNode( selectedTask );
    if ( par == 0 || child == 0 || ! m_project->legalToLink( par, child ) ) {
        return;
    }
    Relation *rel = new Relation ( par, child );
    AddRelationCmd *addCmd = new AddRelationCmd( *m_project,rel,currentItem->text( 0 ) );
    emit executeCommand( addCmd );

}

void PertEditor::slotRemoveClicked()
{
    if ( ! isReadWrite() ) {
        return;
    }
    Node *n = 0;
    Relation *r = m_requiredList->currentRelation();
    if ( r ) {
        n = r->parent();
    }
    removeTaskFromRequiredList();
    setAvailableItemEnabled( n );
}

void PertEditor::removeTaskFromRequiredList()
{
    //kDebug();
    Relation *r = m_requiredList->currentRelation();
    if ( r == 0 ) {
        return;
    }
    // remove the relation
    emit executeCommand( new DeleteRelationCmd( *m_project, r, i18n( "Remove task dependency" ) ) );
}

void PertEditor::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeAdded( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotNodeMoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
        disconnect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeAdded( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotNodeMoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
        connect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
    }
    m_requiredList->setProject( project );
    draw();
}

void PertEditor::slotRelationAdded( Relation *rel )
{
    kDebug()<<rel;
    if ( rel->child() == itemToNode( m_tasktree->currentItem() ) ) {
        QTreeWidgetItem *item = findNodeItem( rel->parent(), m_availableList->invisibleRootItem() );
        updateAvailableTasks( item );
    }
}

void PertEditor::slotRelationRemoved( Relation *rel )
{
    kDebug()<<rel;
    if ( rel->child() == itemToNode( m_tasktree->currentItem() ) ) {
        QTreeWidgetItem *item = findNodeItem( rel->parent(), m_availableList->invisibleRootItem() );
        updateAvailableTasks( item );
    }
}

void PertEditor::slotNodeAdded( Node *node )
{
    kDebug()<<node->name()<<node->childNodeIterator();
    Node *parent = node->parentNode();
    int index = parent->indexOf( node );
    QTreeWidgetItem *pitem = findNodeItem( parent, m_tasktree->invisibleRootItem() );
    if ( pitem == 0 ) {
        pitem = m_tasktree->invisibleRootItem();
    }
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText( 0, node->name() );
    item->setData( 0, NodeRole, node->id() );
    pitem->insertChild( index, item );
    
    pitem = findNodeItem( parent, m_availableList->invisibleRootItem() );
    if ( pitem == 0 ) {
        pitem = m_availableList->invisibleRootItem();
    }
    item = new QTreeWidgetItem();
    item->setText( 0, node->name() );
    item->setData( 0, NodeRole, node->id() );
    item->setData( 0, EnabledRole, true );
    pitem->insertChild( index, item );
    setAvailableItemEnabled( item );
}

void PertEditor::slotNodeRemoved( Node *node )
{
    //kDebug();
    QTreeWidgetItem *item = findNodeItem( node, m_tasktree->invisibleRootItem() );
    if ( item ) {
        QTreeWidgetItem *parent = item->parent();
        if ( parent == 0 ) {
            parent = m_tasktree->invisibleRootItem();
        }
        Q_ASSERT( parent );
        parent->removeChild( item );
        delete item;
    }
    item = findNodeItem( node, m_availableList->invisibleRootItem() );
    if ( item ) {
        QTreeWidgetItem *parent = item->parent();
        if ( parent == 0 ) {
            parent = m_availableList->invisibleRootItem();
        }
        Q_ASSERT( parent );
        parent->removeChild( item );
        delete item;
    }
}

void PertEditor::slotNodeMoved( Node *node )
{
    //kDebug();
    draw();
}

void PertEditor::slotNodeChanged( Node *node )
{
    QTreeWidgetItem *item = findNodeItem( node, m_tasktree->invisibleRootItem() );
    if ( item ) {
        item->setText( 0, node->name() );
    }
    item = findNodeItem( node, m_availableList->invisibleRootItem() );
    if ( item ) {
        item->setText( 0, node->name() );
    }
}

void PertEditor::draw( Project &project)
{
    setProject( &project );
    draw();
}

void PertEditor::draw()
{
    m_tasktree->clear();
    if ( m_project == 0 ) {
        return;
    }
    drawSubTasksName( m_tasktree->invisibleRootItem(), m_project );
}

void PertEditor::drawSubTasksName( QTreeWidgetItem *parent, Node * currentNode)
{
    foreach(Node * currentChild, currentNode->childNodeIterator()){
        QTreeWidgetItem * item = new QTreeWidgetItem( parent );
        item->setText( 0, currentChild->name());
        item->setData( 0, NodeRole, currentChild->id() );
        //kDebug()<<"Added task"<<currentChild->name()<<"parent"<<currentChild->parent();
        drawSubTasksName( item, currentChild);
    }
}

void PertEditor::updateReadWrite( bool rw )
{
    qDebug()<<"PertEditor::updateReadWrite:"<<rw;
    m_requiredList->setReadWrite( rw );
    ViewBase::updateReadWrite( rw );
}

QTreeWidgetItem *PertEditor::findNodeItem( Node *node, QTreeWidgetItem *item ) {
    if ( node->id() == item->data( 0, NodeRole ).toString() ) {
        return item;
    }
    for ( int i = 0; i < item->childCount(); ++i ) {
        QTreeWidgetItem *itm = findNodeItem( node, item->child( i ) );
        if ( itm != 0 ) {
            return itm;
        }
    }
    return 0;
}


void PertEditor::dispAvailableTasks( Relation *rel ){
    dispAvailableTasks();
}

void PertEditor::dispAvailableTasks( Node *parent, Node *selectedTask )
{
    QTreeWidgetItem *pitem = findNodeItem( parent, m_availableList->invisibleRootItem() );
    if ( pitem == 0 ) {
        pitem = m_availableList->invisibleRootItem();
    }
    foreach(Node * currentNode, parent->childNodeIterator() )
    {
        //kDebug()<<currentNode->name()<<"level="<<currentNode->level();
        QTreeWidgetItem *item = new QTreeWidgetItem( QStringList()<<currentNode->name() );
        item->setData( 0, NodeRole, currentNode->id() );
        pitem->addChild(item);
        // Checks it isn't the same as the selected task in the m_tasktree
        setAvailableItemEnabled( item );
        dispAvailableTasks( currentNode, selectedTask );
    }
}

void PertEditor::dispAvailableTasks()
{
    m_availableList->clear();

    if ( m_project == 0 ) {
        return;
    }
    Node *selectedTask = itemToNode( m_tasktree->currentItem() );
    
    loadRequiredTasksList(selectedTask);
    
    dispAvailableTasks( m_project, selectedTask );
}

void PertEditor::updateAvailableTasks( QTreeWidgetItem *item )
{
    //kDebug()<<m_project<<item;
    if ( m_project == 0 ) {
        return;
    }
    if ( item == 0 ) {
        item = m_availableList->invisibleRootItem();
    } else {
        setAvailableItemEnabled( item );
    }
    for ( int i=0; i < item->childCount(); ++i ) {
        updateAvailableTasks( item->child( i ) );
    }
}

void PertEditor::setAvailableItemEnabled( QTreeWidgetItem *item )
{
    //kDebug()<<item;
    Node *node = itemToNode( item );
    if ( node == 0 ) {
        return;
    }
    
    Node *selected = itemToNode( m_tasktree->currentItem() );
    if ( selected == 0 || ! m_project->legalToLink( node, selected ) ) {
        //kDebug()<<"Disable:"<<node->name();
        item->setData( 0, EnabledRole, false );
        QFont f = item->font( 0 );
        f.setItalic( true );
        item->setFont( 0, f );
    } else {
        //kDebug()<<"Enable:"<<node->name();
        item->setData( 0, EnabledRole, true );
        QFont f = item->font( 0 );
        f.setItalic( false );
        item->setFont( 0, f );
    }
    slotAvailableChanged( item );
}

void PertEditor::setAvailableItemEnabled( Node *node )
{
    //kDebug()<<node->name();
    setAvailableItemEnabled( nodeToItem( node, m_availableList->invisibleRootItem() ) );
}

QTreeWidgetItem *PertEditor::nodeToItem( Node *node, QTreeWidgetItem *item )
{
    if ( itemToNode( item ) == node ) {
        return item;
    }
    for ( int i=0; i < item->childCount(); ++i ) {
        QTreeWidgetItem *itm = nodeToItem( node, item->child( i ) );
        if ( itm ) {
            return itm;
        }
    }
    return 0;
}

Node * PertEditor::itemToNode( QTreeWidgetItem *item )
{
    if ( m_project == 0 || item == 0 ) {
        return 0;
    }
    return m_project->findNode( item->data( 0, NodeRole ).toString() );
}

void PertEditor::loadRequiredTasksList(Node * taskNode)
{
    slotRequiredChanged( QModelIndex() );
    m_requiredList->setNode( taskNode );
}

void PertEditor::slotUpdate()
{
 draw();
}

} // namespace KPlato

#include "kptperteditor.moc"
