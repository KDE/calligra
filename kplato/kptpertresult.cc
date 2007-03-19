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
#include "kptpertresult.h"

namespace KPlato
{

void PertResult::draw( Project &project)
{
    widget.treeWidgetTaskResult->clear();
    foreach(Node * currentNode, project.projectNode()->childNodeIterator()){
        if (currentNode->type()!=4){

            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->name());
	    item->setText(1,getStartEarlyDate(currentNode));
	    item->setText(2,getFinishEarlyDate(currentNode));
	    item->setText(3,getStartLateDate(currentNode));
	    item->setText(4,getFinishLateDate(currentNode));
	    item->setText(5,getFreeMargin(currentNode));
	    item->setText(6,getFreeMargin(currentNode));
        }
	widget.labelResultProjectFloat->setText(getProjectFloat(project));
    }
}

QString & PertResult::getStartEarlyDate(Node * currentNode)
{
 return currentNode->name();
}

QString & PertResult::getFinishEarlyDate(Node * currentNode)
{
 return currentNode->name();
}

QString & PertResult::getStartLateDate(Node * currentNode)
{
 return currentNode->name();
}

QString & PertResult::getFinishLateDate(Node * currentNode)
{
 return currentNode->name();
}

QString PertResult::getProjectFloat(Project &project)
{
    QString p_float="Project Float";
    return p_float;
}

QString & PertResult::getFreeMargin(Node * currentNode)
{
 return currentNode->name();
}

QString & PertResult::getTaskFloat(Node * currentNode)
{
 return currentNode->name();
}

//-----------------------------------
PertResult::PertResult( Part *part, QWidget *parent ) : ViewBase( part, parent )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------" << endl;
    widget.setupUi(this);
    QHeaderView *header=widget.treeWidgetTaskResult->header();

    m_part = part;
    m_node = m_part->getProject().projectNode();
	

    (*header).resizeSection(0,200);
    (*header).resizeSection(1,78);
    (*header).resizeSection(2,78);
    (*header).resizeSection(3,78);
    (*header).resizeSection(4,78);
    (*header).resizeSection(5,78);
    (*header).resizeSection(5,78);
    draw( part->getProject() );

}

void PertResult::updateDurationForward()
{
Duration duree;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	for (QList<Relation*>::iterator it=currentNode->dependParentNodes().begin();it!=currentNode->dependParentNodes().end();it++) 
   	{
		duree=(*it)->getmParent()->getmDurationForward() +((*it)->getmLag());
		if (duree>(currentNode->getmDurationForward()))
		{
			currentNode->getmDurationForward()=duree;
		}

  	}
    }
}

QList<Node*> PertResult::criticalPath(Node * currentNode)
{
    //Node * currentNode = m_node->chilNodeIterator().end(); 
    for(QList<Relation*>::iterator it=currentNode->dependParentNodes().end();it!=currentNode->dependParentNodes().begin();it--)
    {
	 if((currentNode->getmDurationForward() -((*it)->getmLag()))==((*it)->getmParent()->getmDurationForward()))
         {
		criticalPath((*it)->getmParent());
		m_criticalPath.push_back((*it)->getmParent());
         }
    }
}



} // namespace KPlato

#include "kptpertresult.moc"
