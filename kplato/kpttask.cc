/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpttask.h"
#include "kptproject.h"

#include <qdom.h>
#include <kdebug.h>


KPTTask::KPTTask() : KPTNode(), m_resource() {
    m_resource.setAutoDelete(true);
}


KPTTask::~KPTTask() {
}


KPTDuration *KPTTask::getExpectedDuration() {
    // I have no idea if this is correct...
    KPTDuration *ed= new KPTDuration();
    if(m_effort) {
        ed->add(m_effort->expected());
    } else {
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            KPTNode *node = it.current();
            KPTDuration *childDuration = node->getExpectedDuration();
            ed->add(*childDuration);
            delete childDuration;
        }
    }
    return ed;
}


KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}


KPTDuration *KPTTask::getStartTime() {
    if(m_startTime == KPTDuration()) {
        // starttime not set, ask our parents!
        // TODO
        return 0L;
    } else {
        return new KPTDuration(m_startTime);
    }
}


KPTDuration *KPTTask::getFloat() {
    return 0L;
}


void KPTTask::addResource(KPTResourceGroup * /* resource */) {
}


void KPTTask::removeResource(KPTResourceGroup * /* resource */){
   // always auto remove
}


void KPTTask::removeResource(int /* number */){
   // always auto remove
}


void KPTTask::insertResource( unsigned int /* index */,
			      KPTResourceGroup * /* resource */) {
}


bool KPTTask::load(QDomElement &element) {
    // Maybe TODO: Delete old stuff here

    // Load attributes (TODO: Finish with the rest of them)
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

    // Load the project children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
	if (list.item(i).isElement()) {
	    QDomElement e = list.item(i).toElement();

	    if (e.tagName() == "project") {
		// Load the subproject
		KPTProject *child = new KPTProject();
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "task") {
		// Load the task
		KPTTask *child = new KPTTask();
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "milestone") {
		// TODO: Load the milestone
	    } else if (e.tagName() == "terminalnode") {
		// TODO: Load the terminalnode
	    } else if (e.tagName() == "relation") {
		// TODO: Load the relation
	    } else if (e.tagName() == "resource") {
		// TODO: Load the resource
	    }
	}
    }

    return true;
}


void KPTTask::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    // TODO: Save the rest of the information
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    for (int i=0; i<numChildren(); i++)
	// First add the child
	getChildNode(i).save(me);
}
