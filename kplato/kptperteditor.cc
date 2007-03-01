/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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

namespace KPlato
{

void PertEditor::draw( Project &project)
{
    m_tasktree->clear();
    
    foreach(Node * currentNode, project.projectNode()->childNodeIterator()){
        if (currentNode->type()!=4){
            QTreeWidgetItem * item = new QTreeWidgetItem( m_tasktree );
            item->setText( 0, currentNode->name());
            drawSubTasksName(item,currentNode);
        }
        //kDebug() << "[void KPlato::PertEditor::draw( Project &project )] TASK FOUNDED" << endl;
    }

}

void PertEditor::drawSubTasksName( QTreeWidgetItem *parent, Node * currentNode)
{
    if (currentNode->numChildren() > 0){
        foreach(Node * currentChild, currentNode->childNodeIterator()){
            if (currentChild->type()!=4){
                QTreeWidgetItem * item = new QTreeWidgetItem( parent );
                item->setText( 0, currentChild->name());
                drawSubTasksName( item, currentChild);
            }
            //kDebug() << "[void KPlato::PertEditor::draw( Project &project )] SUBTASK FOUNDED" << endl;
        }
    }
}


//-----------------------------------
PertEditor::PertEditor( Part *part, QWidget *parent ) : ViewBase( part, parent )
{
    kDebug() << " ---------------- KPlato: Creating PertEditor ----------------" << endl;
    widget.setupUi(this);
    widget.assignList->setSelectedLabel(i18n("Required Tasks :"));
    widget.assignList->setAvailableLabel(i18n("Available Tasks :"));
    widget.assignList->setShowUpDownButtons(false);
    widget.assignList->layout()->setMargin(0);

    m_tasktree = widget.tableTaskWidget;
    m_assignList = widget.assignList;
    m_part = part;
    m_node = m_part->getProject().projectNode();
    
    draw( part->getProject() );  
    for (int i=0;i< list_nodeNotView.size();i++)
	list_nodeNotView.removeFirst();
	
    connect( m_tasktree, SIGNAL( itemSelectionChanged() ), SLOT( dispAvailableTasks() ) );
    connect( m_assignList, SIGNAL(added(QListWidgetItem *)), this, SLOT(addTaskInRequiredList(QListWidgetItem * )) );
    connect( m_assignList, SIGNAL(removed(QListWidgetItem *)), this, SLOT(removeTaskFromRequiredList(QListWidgetItem * )) );
}

void PertEditor::dispAvailableTasks(){
    
    for (int i=0;i< list_nodeNotView.size();i++)
	list_nodeNotView.removeFirst();

    list_nodeNotView.begin();
    QString selectedTaskName = m_tasktree->selectedItems().first()->text(0);

    m_assignList->availableListWidget()->clear();
    m_assignList->selectedListWidget()->clear();

    loadRequiredTasksList(itemToNode(selectedTaskName, m_node));
    
    listNodeNotView(itemToNode(selectedTaskName, m_node));
    
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
        // Checks if the curent node is not a milestone
        // and if it isn't the same as the selected task in the m_tasktree
	
	if ( currentNode->type() != 4 and currentNode->name() != selectedTaskName
	       and  !list_nodeNotView.contains(currentNode) 	
               and (m_assignList->selectedListWidget()->findItems(currentNode->name(),0)).empty())
	{
            m_assignList->availableListWidget()->addItem(currentNode->name());
        }
    }
    //remove all nodes from list_nodeParent
    for (int i=0;i< list_nodeNotView.size();i++)
    {
	list_nodeNotView.removeFirst();
    }
    list_nodeNotView.begin();
}




//return parents of the node
QList<Node*> PertEditor::listNodeNotView(Node * node)
{
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




Node * PertEditor::itemToNode(QString itemName, Node * startNode){
    Node * result;
    if (startNode->numChildren() > 0){
        foreach(Node * currentNode, startNode->childNodeIterator() ){
            if (currentNode->name() == itemName) {
                return currentNode;
            } else {
                result=itemToNode(itemName, currentNode);
            }
        }
    }
    return result;
}

void PertEditor::addTaskInRequiredList(QListWidgetItem * currentItem){
    // add the relation between the selected task and the current task
    QString selectedTaskName = m_tasktree->selectedItems().first()->text(0);

    Relation* m_rel=new Relation (itemToNode(currentItem->text(), m_node),itemToNode(selectedTaskName, m_node));
    AddRelationCmd * addCmd= new AddRelationCmd(m_part,m_rel,currentItem->text());
    m_part->addCommand( addCmd );
}

void PertEditor::removeTaskFromRequiredList(QListWidgetItem * currentItem){
    // remove the relation between the selected task and the current task
    QString selectedTaskName = m_tasktree->selectedItems().first()->text(0);

    Relation* m_rel = itemToNode(selectedTaskName, m_node)->findParentRelation(itemToNode(currentItem->text(), m_node));
    DeleteRelationCmd * delCmd= new DeleteRelationCmd(m_part,m_rel,currentItem->text());
    m_part->addCommand( delCmd );
}

void PertEditor::loadRequiredTasksList(Node * taskNode){
    // Display the required task list into the rigth side of m_assignList
    m_assignList->selectedListWidget()->clear();
    foreach(Relation * currentRelation, taskNode->dependParentNodes()){
            m_assignList->selectedListWidget()->addItem(currentRelation->parent()->name());
        }
}


} // namespace KPlato

#include "kptperteditor.moc"
