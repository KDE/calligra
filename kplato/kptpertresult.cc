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
    QList<Node*> list;
    QString res;

    foreach(Node * currentNode, project.projectNode()->childNodeIterator()){
        if (currentNode->type()!=4){
 
            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->name());
	    item->setText(1,locale->formatDateTime(getStartEarlyDate(currentNode)));
	    item->setText(2,locale->formatDateTime(getFinishEarlyDate(currentNode)));
	    item->setText(3,locale->formatDateTime(getStartLateDate(currentNode)));
	    item->setText(4,locale->formatDateTime(getFinishLateDate(currentNode)));
	    item->setText(5,res.number(getTaskFloat(currentNode).days()));
	    item->setText(6,res.number(getFreeMargin(currentNode).days()));
        }
	widget.labelResultProjectFloat->setText(res.number(getProjectFloat(project).days()));

    }
    list=criticalPath();
    QList<Node*>::iterator it=list.begin();
    while(it!=list.end()) 
    {
	 res+=(*it)->id();
	 it++;
	 if(it!=list.end()) res+=" - ";
    }
    widget.labelResultCriticalPath->setText(res);
}

DateTime PertResult::getStartEarlyDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    //if the task has no parent so the early date start is 0
    if(currentNode->dependParentNodes().size()==0)
    {
        t=static_cast<Task *>(currentNode);
        duration=t->startTime();
        duration.setDateOnly(true);
        return duration;
    }
    else
    {
    //if the task have parents so we add duration of all parent task
    	for (QList<Relation*>::iterator it=currentNode->dependParentNodes().begin();it!=currentNode->dependParentNodes().end();it++)
    	{
            t=static_cast<Task *>((*it)->parent ());
            if(it==currentNode->dependParentNodes().begin())
	    {
	        duration=t->startTime( );
	    }
	duration+=(t->endTime()-t->startTime());
    	}
    duration.setDateOnly(true);
    return duration;
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
    Task * t;
    t=static_cast<Task *>(currentNode);
    return (getFinishLateDate(currentNode)-=(t->endTime()-t->startTime()));

}


DateTime PertResult::getFinishLateDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    QList<DateTime> l;
    if(currentNode->dependChildNodes().size()==0)
    {
        t=static_cast<Task *>(currentNode);
        duration=getStartEarlyDate(currentNode);
	duration.setDateOnly(true);
        return duration;
    }
    else
    {
    	for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    	{
            t=static_cast<Task *>((*it)->child ());
	    duration=getFinishLateDate((*it)->child ());
            duration-=((*it)->child ()->endTime()-(*it)->child ()->startTime());
	    l.push_back(duration);
    	}
    
    	for (QList<DateTime>::iterator it=l.begin();it!=l.end();it++)
    	{
	    if(it==l.begin())
	    {
	    	duration=(*it);
	    }
	    if((*it)<duration)
	    {
		duration=(*it);
	    }
	}
    duration.setDateOnly(true);
    return duration;
    }
}

Duration PertResult::getProjectFloat(Project &project)
{
    Duration duration;
    foreach(Node * currentNode, project.projectNode()->childNodeIterator() )
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
    if(currentNode->dependChildNodes().size()==0)
    {
         return getFinishLateDate(currentNode)-getStartEarlyDate(currentNode);
    }
    else
    {
        return getFinishLateDate(currentNode)-getFinishEarlyDate(currentNode);
    }
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



} // namespace KPlato

#include "kptpertresult.moc"
