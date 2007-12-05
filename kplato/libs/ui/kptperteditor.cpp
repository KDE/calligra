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

#include <KoDocument.h>

namespace KPlato
{

void PertEditor::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( relationRemoved(Relation *) ), this, SLOT( dispAvailableTasks(Relation *) ) );
        disconnect( m_project, SIGNAL( relationAdded(Relation *) ), this, SLOT( dispAvailableTasks(Relation *) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
    m_project = project;
    m_node = project; // TODO: review
    if ( m_project ) {
        connect( m_project, SIGNAL( relationRemoved(Relation *) ), this, SLOT( dispAvailableTasks(Relation *) ) );
        connect( m_project, SIGNAL( relationAdded(Relation *) ), this, SLOT( dispAvailableTasks(Relation *) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
    draw();
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
    
    foreach(Node *currentNode, m_project->childNodeIterator()){
        kDebug() << currentNode->type();
        QTreeWidgetItem * item = new QTreeWidgetItem( m_tasktree );
        item->setText( 0, currentNode->name());
        item->setData( 0, Qt::UserRole + 1, currentNode->id() );
        drawSubTasksName(item,currentNode);
        //kDebug() << "TASK FOUND";
    }
}

void PertEditor::drawSubTasksName( QTreeWidgetItem *parent, Node * currentNode)
{
    foreach(Node * currentChild, currentNode->childNodeIterator()){
        QTreeWidgetItem * item = new QTreeWidgetItem( parent );
        item->setText( 0, currentChild->name());
        item->setData( 0, Qt::UserRole + 1, currentChild->id() );
        drawSubTasksName( item, currentChild);
        //kDebug() << SUBTASK FOUND";
    }
}


//-----------------------------------
PertEditor::PertEditor( KoDocument *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_node( 0 ),
    m_project( 0 )
{
    kDebug() <<" ---------------- KPlato: Creating PertEditor ----------------";
    widget.setupUi(this);
    widget.assignList->setSelectedLabel(i18n("Required Tasks :"));
    widget.assignList->setAvailableLabel(i18n("Available Tasks :"));
    widget.assignList->setShowUpDownButtons(false);
    widget.assignList->layout()->setMargin(0);
    widget.assignList->setMoveOnDoubleClick(false);

    m_tasktree = widget.tableTaskWidget;
    m_assignList = widget.assignList;
    m_part = part;
    
    for (int i=0;i< list_nodeNotView.size();i++)
        list_nodeNotView.removeFirst();

    connect( m_tasktree, SIGNAL( itemSelectionChanged() ), SLOT( dispAvailableTasks() ) );
    connect( this, SIGNAL( refreshAvailableTaskList() ), SLOT( dispAvailableTasks() ) );
    connect( m_assignList, SIGNAL(added(QListWidgetItem *)), this, SLOT(addTaskInRequiredList(QListWidgetItem * )) );
    connect( m_assignList, SIGNAL(removed(QListWidgetItem *)), this, SLOT(removeTaskFromRequiredList(QListWidgetItem * )) );

    m_assignList->setAvailableInsertionPolicy( KActionSelector::AtBottom );
    //m_assignList->setButtonsEnabled();
}

void PertEditor::updateReadWrite( bool rw )
{
    //TODO
}

void PertEditor::dispAvailableTasks( Relation *rel ){
    dispAvailableTasks();
}

void PertEditor::dispAvailableTasks( Node *parent, Node *selectedTask )
{
    foreach(Node * currentNode, parent->childNodeIterator() )
    {
        //TODO: we need to use QTreeWidget instead of QListWidget
        QString indent = QString( "%1" ).arg( "", (currentNode->level()-1)*2 );
        kDebug()<<indent+currentNode->name()<<"level="<<currentNode->level();
        // Checks it isn't the same as the selected task in the m_tasktree
        if ( selectedTask != 0 && currentNode != selectedTask
             && (m_assignList->selectedListWidget()->findItems(currentNode->name(),0)).empty()
             && m_project->legalToLink( currentNode, selectedTask ) )
        {
            QListWidgetItem *item = new QListWidgetItem(indent+currentNode->name());
            item->setData( Qt::UserRole + 1, currentNode->id() );
            m_assignList->availableListWidget()->addItem(item);
        }
        else
        {
            QFont* fakeItemFont = new QFont();
            QBrush* fakeItemBrush = new QBrush();
            fakeItemFont->setItalic(true);
            fakeItemBrush->setColor(QColor ( Qt::lightGray));
            QListWidgetItem* fakeItem = new QListWidgetItem();
            fakeItem->setData( Qt::UserRole + 1, currentNode->id() );
            fakeItem->setText(indent+currentNode->name());
            fakeItem->setFont(*fakeItemFont);
            fakeItem->setForeground(*fakeItemBrush);
            fakeItem->setFlags(Qt::ItemIsEnabled);
            m_assignList->availableListWidget()->addItem(fakeItem);
        }
        dispAvailableTasks( currentNode, selectedTask );
    }
    //m_assignList->setButtonsEnabled();
}

void PertEditor::dispAvailableTasks(){

    list_nodeNotView.clear();

//    list_nodeNotView.begin();

    QList<QTreeWidgetItem*> selectedItemList=m_tasktree->selectedItems();
    
    if ( m_project == 0 ) {
        return;
    }
    if(!(selectedItemList.count()<1))
    {
        Node *selectedTask = itemToNode(m_tasktree->selectedItems().first());
        m_assignList->availableListWidget()->clear();
        m_assignList->selectedListWidget()->clear();
        
        loadRequiredTasksList(selectedTask);
        
//        listNodeNotView(selectedTask);
        
        dispAvailableTasks( m_project, selectedTask );
        
        list_nodeNotView.clear();
    }
    else
    {
        m_assignList->availableListWidget()->clear();
        m_assignList->selectedListWidget()->clear();
    }
}

//return parents of the node
QList<Node*> PertEditor::listNodeNotView(Node * node)
{
    if ( m_project == 0 ) {
        return QList<Node*>();
    }
    list_nodeNotView = node->getParentNodes();
    foreach(Node* currentNode,m_node->childNodeIterator())
    {
        if (currentNode->isDependChildOf(node))
        {
            list_nodeNotView.append(currentNode);
        }
    }
    return list_nodeNotView;
}


Node * PertEditor::itemToNode( QTreeWidgetItem *item )
{
    if ( m_project == 0 ) {
        return 0;
    }
    return m_project->findNode( item->data( 0, Qt::UserRole + 1 ).toString() );
}

Node * PertEditor::itemToNode( QListWidgetItem *item )
{
    if ( m_project == 0 ) {
        return 0;
    }
    return m_project->findNode( item->data( Qt::UserRole + 1 ).toString() );
}

void PertEditor::addTaskInRequiredList(QListWidgetItem * currentItem)
{
    kDebug()<<currentItem;
    if ( currentItem == 0 ) {
        return;
    }
    if ( m_project == 0 ) {
        return;
    }
    // add the relation between the selected task and the current task
    QTreeWidgetItem *selectedTask = m_tasktree->selectedItems().first();
    if ( selectedTask == 0 ) {
        return;
    }

    Node *par = itemToNode( currentItem );
    Node *child = itemToNode( selectedTask );
    if ( par == 0 || child == 0 ) {
        return;
    }
    Relation *m_rel = new Relation ( par, child );
    AddRelationCmd * addCmd= new AddRelationCmd(*m_project,m_rel,currentItem->text());
    m_part->addCommand( addCmd );

}

void PertEditor::removeTaskFromRequiredList(QListWidgetItem * currentItem){
    kDebug()<<currentItem;
    if ( currentItem == 0 ) {
        return;
    }
    // remove the relation between the selected task and the current task
    QTreeWidgetItem *selectedTask = m_tasktree->selectedItems().first();
    // remove the relation
    Relation* m_rel = itemToNode(selectedTask)->findParentRelation(itemToNode(currentItem));
    DeleteRelationCmd * delCmd= new DeleteRelationCmd(*m_project,m_rel,currentItem->text());
    m_part->addCommand( delCmd );

//    dispAvailableTasks();
}

void PertEditor::loadRequiredTasksList(Node * taskNode){
    if ( taskNode == 0 ) {
        kWarning()<<"No current node";
        return;
    }
    // Display the required task list into the rigth side of m_assignList
    m_assignList->selectedListWidget()->clear();
    foreach(Relation * currentRelation, taskNode->dependParentNodes()){
        QListWidgetItem *item = new QListWidgetItem(currentRelation->parent()->name());
        item->setData( Qt::UserRole + 1, currentRelation->parent()->id() );
        m_assignList->selectedListWidget()->addItem(item);
    }
}

void PertEditor::slotUpdate(){

 draw();
}

} // namespace KPlato

#include "kptperteditor.moc"
