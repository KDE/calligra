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

#include "kptmilestone.h"
#include "kptmilestonedialog.h"
#include "kpttimescale.h"
#include "kptpertcanvas.h"
#include "kptcanvasitem.h"

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

KPTMilestone::KPTMilestone(KPTNode *parent) : KPTNode(parent) {
}

KPTMilestone::~KPTMilestone() {
}

bool KPTMilestone::load(QDomElement &element) {
    kdDebug()<<k_funcinfo<<endl;
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

    earliestStart = KPTDuration(QDateTime::fromString(element.attribute("earlieststart")));
    latestFinish = KPTDuration(QDateTime::fromString(element.attribute("latestfinish")));

    return true;
}

void KPTMilestone::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("milestone");
    element.appendChild(me);

    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("earlieststart",earliestStart.dateTime().toString());
    me.setAttribute("latestfinish",latestFinish.dateTime().toString());
    
    // Only save parent relations    
    QPtrListIterator<KPTRelation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

}

KPTDuration *KPTMilestone::getStartTime() {
    return new KPTDuration(earliestStart);
}

KPTDuration *KPTMilestone::getFloat() {
    return 0L;
}


bool KPTMilestone::openDialog() {
    kdDebug()<<k_funcinfo<<endl;
    KPTMilestoneDialog *dialog = new KPTMilestoneDialog(*this);
    bool ret = dialog->exec();
    delete dialog;
    return ret;
}

void KPTMilestone::drawGanttBar(QCanvas* canvas,KPTTimeScale* ts, int y, int h) {
    kdDebug()<<k_funcinfo<<endl;
    m_y = y;
    m_h = h;
        
    // Draw slack
    QCanvasLine *l = new QCanvasLine(canvas);
    l->setPoints(ts->getX(&earliestStart),y+h/2,ts->getX(&latestFinish),y+h/2);
    l->setPen( QPen(Qt::blue) );
    l->setZ(40);
    l->show();
    // Draw bar
    QBrush brush(Qt::black);
    KPTDuration *start = getStartTime();
    int x = ts->getX(start);
    QPointArray a(5);
    a.setPoint(0, x, y+h/2);
    a.setPoint(1, x+h/2, y);
    a.setPoint(2, x+h, y+h/2);
    a.setPoint(3, x+h/2, y+h);
    a.setPoint(4, a.point(0));
    QCanvasPolygon *p = new QCanvasPolygon(canvas);
    p->setPoints(a);
    p->setBrush( brush );
    p->setZ(50);
    p->show();
    delete start;
}
void KPTMilestone::drawPert(KPTPertCanvas *view, QCanvas* canvas, int col) {
	if ( numChildren() > 0 ) {
	    QPtrListIterator<KPTNode> nit(m_nodes); 
		for ( ; nit.current(); ++nit ) {
		    nit.current()->drawPert(view, canvas, col);
		}
    } else {
		if (!m_drawn) {
            col = parentColumn() +1;
		    int row = view->row(col);
            m_pertItem = new KPTPertCanvasItem(canvas, *this, row, col);
			m_pertItem->show();
			m_drawn = true;
			view->setRow(++row, col);
	    }
        QPtrListIterator<KPTRelation> cit(m_dependChildNodes);
		for ( ; cit.current(); ++cit ) {
		    cit.current()->child()->drawPert(view, canvas, col+1);
		}
	}
}

void KPTMilestone::drawPertRelations(QCanvas* canvas) {
    kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTRelation> it(m_dependChildNodes); 
    for ( ; it.current(); ++it ) {
        it.current()->draw(canvas);
    }
}

#ifndef NDEBUG
void KPTMilestone::printDebug(bool children, QCString indent) {
    QCString i2 = indent + "---";
    kdDebug()<<indent<<" Milestone: "<<name()<<endl;
    KPTNode::printDebug(children, indent);
        
}
#endif
