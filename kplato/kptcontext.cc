/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptcontext.h"

#include <qdom.h>

#include <kdebug.h>

namespace KPlato
{

Context::Context() {
}

Context::~Context() {
}

bool Context::load(QDomElement &element) {
    currentView = element.attribute("current-view");
    
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();    
            if (e.tagName() == "gantt-view") {
                ganttviewsize = e.attribute("ganttview-size").toInt();
                taskviewsize = e.attribute("taskview-size").toInt();
                currentNode = e.attribute("current-node");
                showResources = e.attribute("show-resources").toInt();
                showTaskName = e.attribute("show-taskname").toInt();
                showTaskLinks = e.attribute("show-tasklinks").toInt();
                showProgress = e.attribute("show-progress").toInt();
                showPositiveFloat = e.attribute("show-positivefloat").toInt();
                showCriticalTasks = e.attribute("show-criticaltasks").toInt();
                showCriticalPath = e.attribute("show-criticalpath").toInt();
                
                QDomNodeList list = e.childNodes();
                for (unsigned int i=0; i<list.count(); ++i) {
                    if (list.item(i).isElement()) {
                        QDomElement g = list.item(i).toElement();
                        if (g.tagName() == "closed-nodes") {
                            QDomNodeList list = g.childNodes();
                            for (unsigned int i=0; i<list.count(); ++i) {
                                if (list.item(i).isElement()) {
                                    QDomElement ei = list.item(i).toElement();
                                    if (ei.tagName() == "node") {
                                        closedNodes.append(ei.attribute("id"));
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                kdError()<<k_funcinfo<<"Unknown tag: "<<e.tagName()<<endl;
            }
        }
    }
    return true;
}

void Context::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("context");
    element.appendChild(me);
    me.setAttribute("current-view", currentView);
    // Ganttview
    QDomElement g = me.ownerDocument().createElement("gantt-view");
    me.appendChild(g);
    g.setAttribute("ganttview-size", ganttviewsize);
    g.setAttribute("taskview-size", taskviewsize);
    g.setAttribute("current-node", currentNode);
    g.setAttribute("show-resources", showResources);
    g.setAttribute("show-taskname", showTaskName);
    g.setAttribute("show-tasklinks", showTaskLinks);
    g.setAttribute("show-progress", showProgress);
    g.setAttribute("show-positivefloat", showPositiveFloat);
    g.setAttribute("show-criticaltasks", showCriticalTasks);
    g.setAttribute("show-criticalpath", showCriticalPath);
    if (!closedNodes.isEmpty()) {
        QDomElement e = g.ownerDocument().createElement("closed-nodes");
        g.appendChild(e);
        for (QStringList::ConstIterator it = closedNodes.begin(); it != closedNodes.end(); ++it) {
            QDomElement c = e.ownerDocument().createElement("node");
            e.appendChild(c);
            c.setAttribute("id", (*it));
        }
    }
}

}  //KPlato namespace
