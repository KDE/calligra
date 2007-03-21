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
#include <klocale.h>

namespace KPlato
{

void PertResult::draw( Project &project)
{
    widget.treeWidgetTaskResult->clear();
    KLocale * locale = KGlobal::locale();
    updateDurationForward();
    foreach(Node * currentNode, project.projectNode()->childNodeIterator()){
        if (currentNode->type()!=4){

            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->name());
	    item->setText(1,locale->formatDateTime(getStartEarlyDate(currentNode)));
	    item->setText(2,locale->formatDateTime(getFinishEarlyDate(currentNode)));
	    item->setText(3,locale->formatDateTime(getStartLateDate(currentNode)));
	    item->setText(4,locale->formatDateTime(getFinishLateDate(currentNode)));
	    item->setText(5,getFreeMargin(currentNode).toString());
	    item->setText(6,getFreeMargin(currentNode).toString());
        }
	widget.labelResultProjectFloat->setText(getProjectFloat(project).toString());
    }
}

DateTime PertResult::getStartEarlyDate(Node * currentNode)
{
    DateTime duree;
    Task * t;
    //if the task has no parent so the early date start is 0
    if(currentNode->dependParentNodes().size()==0)
    {
        t=static_cast<Task *>(currentNode);
        duree=t->startTime();
        return duree;
    }
    else
    {
    //if the task have parents so we add duration of all parent task
    	for (QList<Relation*>::iterator it=currentNode->dependParentNodes().begin();it!=currentNode->dependParentNodes().end();it++)
    	{
            t=static_cast<Task *>((*it)->parent ());
            if(it==currentNode->dependParentNodes().begin())
	    {
	        duree=t->startTime( );
	    }
	duree+=(t->endTime()-t->startTime());
    	}
    return duree;
    }
}

DateTime PertResult::getFinishEarlyDate(Node * currentNode)
{
    //it's the early start date + duration of the task
    Task * t;
    t=static_cast<Task *>(currentNode);
    return (getStartEarlyDate(currentNode)+=(t->endTime()-t->startTime()));
}
 
DateTime PertResult::getStartLateDate(Node * currentNode)
{
    DateTime duree;
    Task * t;
    if(currentNode->dependChildNodes().size()==0)
    {
        t=static_cast<Task *>(currentNode);
        duree=getStartEarlyDate(currentNode);
        return duree;
    }
    else
    {
    	for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    	{
            t=static_cast<Task *>((*it)->child ());
            if(it==currentNode->dependChildNodes().begin())
	    {
		duree=getStartLateDate((*it)->child ());
	    }
	    if(duree>getStartLateDate((*it)->child ()))
	    {
	        duree=getStartLateDate((*it)->child ());
		kDebug() << "BOUCLE" << endl;
		kDebug() << duree.toString() << endl;
	    }
	    kDebug() << "FIN BOUCLE" << endl;
	    kDebug() << duree.toString() << endl;
    	}
    return duree-=(currentNode->endTime()-currentNode->startTime());
    }
}


DateTime PertResult::getFinishLateDate(Node * currentNode)
{
 //it's the late start date + duration of the task
    Task * t;
    t=static_cast<Task *>(currentNode);
    return (getStartLateDate(currentNode)+=(t->endTime()-t->startTime()));
}

Duration PertResult::getProjectFloat(Project &project)
{
    Duration duree;
    foreach(Node * currentNode, project.projectNode()->childNodeIterator() )
    {
	duree=duree+getTaskFloat(currentNode);
        kDebug() << "FLOAT PROJECT" << endl;
        kDebug() << duree.toString() << endl;
    }
    return duree;
}

Duration PertResult::getFreeMargin(Node * currentNode)
{
    //search the small duration of the nextest task
    Task * t;
    DateTime duree;
    for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    	{
            if(it==currentNode->dependChildNodes().begin())
	    {
		duree=getStartEarlyDate((*it)->child());
	    }
	    t=static_cast<Task *>((*it)->child ());
            if(getStartEarlyDate((*it)->child ())<duree)
	    {
	        duree=getStartEarlyDate((*it)->child ());
	    }
    	}
    t=static_cast<Task *>(currentNode);
    return duree-(getStartEarlyDate(currentNode)+=(t->endTime()-t->startTime())); 
}

Duration PertResult::getTaskFloat(Node * currentNode)
{
 return getStartLateDate(currentNode)-getStartEarlyDate(currentNode);
}

//-----------------------------------
PertResult::PertResult( Part *part, QWidget *parent ) : ViewBase( part, parent )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------" << endl;
    widget.setupUi(this);
    QHeaderView *header=widget.treeWidgetTaskResult->header();

    m_part = part;
    m_node = m_part->getProject().projectNode();
	

    (*header).resizeSection(0,120);
    (*header).resizeSection(1,110);
    (*header).resizeSection(2,110);
    (*header).resizeSection(3,110);
    (*header).resizeSection(4,110);
    (*header).resizeSection(5,80);
    (*header).resizeSection(5,80);
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
		//kDebug() <<duree.toString()<<endl;

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
